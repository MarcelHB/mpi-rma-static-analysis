#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"

#include "phasar/PhasarLLVM/IfdsIde/FlowFunctions/Gen.h"
#include "phasar/PhasarLLVM/IfdsIde/FlowFunctions/Identity.h"
#include "phasar/PhasarLLVM/IfdsIde/FlowFunctions/Kill.h"
#include "phasar/PhasarLLVM/Plugins/Interfaces/IfdsIde/IFDSTabulationProblemPlugin.h"
#include "phasar/Utils/LLVMShorthands.h"

#include "BranchPipe.h"
#include "FactKiller.h"
#include "MPIRMAUseProblem.h"
#include "MPISymbols.h"
#include "SymbolicExecutor.h"
#include "WindowCreator.h"

MPIRMAUseProblem::MPIRMAUseProblem (
    psr::LLVMBasedICFG &ICFG
  , const UnlockMap &UnlockMap
  , std::vector<std::string> &EntryPoints
  , const Settings &Settings
) : MPIRMAIFDSClass(ICFG)
  , entryPoints(EntryPoints)
  , _zeroValue(std::make_unique<D_Type>(Fact::Type::Null, UnlockMap, Settings.ithWindow))
  , integerMode(Settings.integerMode)
{
  DefaultIFDSTabulationProblem::zerovalue = createZeroValue();
}

MPIRMAUseProblem::D_Type* MPIRMAUseProblem::createZeroValue () {
  return _zeroValue.get();
}

std::shared_ptr<psr::FlowFunction<MPIRMAUseProblem::D_Type*>>
MPIRMAUseProblem::getNormalFlowFunction (
    const llvm::Instruction *CurrentInstruction
  , const llvm::Instruction *SuccessorInstruction
) {
  if (auto branchInstruction = llvm::dyn_cast<llvm::BranchInst>(CurrentInstruction)) {
    struct BranchWarner : psr::FlowFunction<D_Type*> {
      const llvm::BranchInst *branchInstruction;

      BranchWarner (const llvm::BranchInst *BranchInst) : branchInstruction(BranchInst) {}

      std::set<D_Type*> computeTargets (D_Type *value) {
        if (  value->isType(Fact::Type::Win)
          ||  value->isType(Fact::Type::IntConst)
        ) {
          value->registerBranch(branchInstruction);
        }

        return {value};
      }
    };

    return std::make_shared<BranchWarner>(branchInstruction);
  }

  // Very common pattern for Fortan.
  if (auto bitcastInstruction = llvm::dyn_cast<llvm::BitCastInst>(CurrentInstruction)) {
    struct BitcastTracer : psr::FlowFunction<D_Type*> {
      const llvm::BitCastInst *bcInst;

      BitcastTracer (
          const llvm::BitCastInst *BC
      ) : bcInst(BC)
      {}

      std::set<D_Type*> computeTargets (D_Type* value) {
        if ( value->isType(Fact::Type::Win)
          || value->isType(Fact::Type::Comm)
          || value->isType(Fact::Type::IntConst)
        ) {
          if (value->knowsValue(bcInst->getOperand(0))) {
            value->addKnownValue(bcInst);
          }
        }

        return {value};
      }
    };

    return std::make_shared<
        BranchPipe<BitcastTracer>
      >(  bitcastInstruction
        , *this
        , bitcastInstruction
      );
  }

  if (auto storeInstruction = llvm::dyn_cast<llvm::StoreInst>(CurrentInstruction)) {
    struct StoreTracer : psr::FlowFunction<D_Type*> {
      const llvm::StoreInst *storeInstruction;
      MPIRMAUseProblem& problem;

      StoreTracer (
          const llvm::StoreInst *S
        , MPIRMAUseProblem &Instance
      ) : storeInstruction(S), problem(Instance)
      {}

      std::set<D_Type*> computeTargets (D_Type* value) {
        // handles (comm, window, int)
        if ( value->isType(Fact::Type::Win)
          || value->isType(Fact::Type::Comm)
          || value->isType(Fact::Type::IntConst)
        ) {
          if (value->knowsValue(storeInstruction->getValueOperand())) {
            value->addKnownValue(storeInstruction->getPointerOperand());
          } else if (
                  value->isType(Fact::Type::IntConst)
              &&  value->isMutable()
              &&  value->knowsValue(storeInstruction->getPointerOperand())
          ) {
            if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(storeInstruction->getValueOperand())) {
              value->setCurrentInteger(constant->getSExtValue());
            } else {
              value->markIntegerUnresolvable();
            }
          }
        }

        if ( value->isType(Fact::Type::Comm)
          || value->isType(Fact::Type::Null)
        ) {
          if (value->knowsRankValue(storeInstruction->getValueOperand())) {
            value->addKnownRankValue(storeInstruction->getPointerOperand());
          }
        }

        // Generate integer constant
        if ( value->isType(Fact::Type::Null)
          && Settings::IntegerMode::None != problem.integerMode
        ) {
          if (auto constant = llvm::dyn_cast<llvm::ConstantInt>(storeInstruction->getValueOperand())) {
            if (constant->getType()->isIntegerTy()) {
              D_Type *fact = problem.makeInitialFact(
                  Fact::Type::IntConst
                , value
                , constant->getSExtValue()
                , Settings::IntegerMode::All != problem.integerMode
              );
              fact->addKnownValue(storeInstruction->getPointerOperand(), true);

              return {value, fact};
            }
          }
        }

        // window memory
        if (value->isType(Fact::Type::Win) && !value->isLocked()) {
          auto path = value->getPath();

          if (value->knowsAddress(storeInstruction->getPointerOperand())) {
            path->transit(
                WindowState::State::LocalWrite
              , storeInstruction
              , MPISymbol::MPILocalWrite
            );
          } else if (value->knowsLocalAddress(storeInstruction->getPointerOperand())) {
            path->transit(
                WindowState::State::LocalWrite
              , storeInstruction
              , MPISymbol::MPIDataWrite
            );
          } else if (value->knowsSharedAddress(storeInstruction->getPointerOperand())) {
            const llvm::CallInst *generator = nullptr;
            auto &valueCallMap = value->getValueCallMap();
            auto searchResult = valueCallMap.find(storeInstruction->getPointerOperand());

            if (valueCallMap.end() != searchResult) {
              if (auto callInst = llvm::dyn_cast<llvm::CallInst>(searchResult->second)) {
                generator = callInst;
              }
            }

            path->transit(
                WindowState::State::RemoteCommunication
              , storeInstruction
              , MPISymbol::MPISharedWrite
              , generator
            );
          } else if (value->knowsAddress(storeInstruction->getValueOperand())) {
            value->addKnownAddress(storeInstruction->getPointerOperand());
          } else if (value->knowsLocalAddress(storeInstruction->getValueOperand())) {
            value->addKnownLocalAddress(
                storeInstruction->getPointerOperand()
              , storeInstruction->getValueOperand()
            );
          } else if (value->knowsSharedAddress(storeInstruction->getValueOperand())) {
            value->addKnownSharedAddress(
                storeInstruction->getPointerOperand()
              , storeInstruction->getValueOperand()
            );
          }
        }

        return {value};
      }
    };

    return std::make_shared<
        BranchPipe<StoreTracer>
      >(  storeInstruction
        , *this
        , storeInstruction
        , *this
      );
  }

  if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(CurrentInstruction)) {
    struct LoadTracer : psr::FlowFunction<D_Type*> {
      const llvm::LoadInst *loadInstruction;

      LoadTracer (const llvm::LoadInst *L) : loadInstruction(L) {}

      std::set<D_Type*> computeTargets (D_Type* value) {
        // handles (comm, window, int)
        if ( value->isType(Fact::Type::Win)
          || value->isType(Fact::Type::Comm)
          || value->isType(Fact::Type::IntConst)
        ) {
          if (value->knowsValue(loadInstruction->getPointerOperand())) {
            value->addKnownValue(loadInstruction);
          }
        }

        if ( value->isType(Fact::Type::Comm)
          || value->isType(Fact::Type::Null)
        ) {
          if (value->knowsRankValue(loadInstruction->getPointerOperand())) {
            value->addKnownRankValue(loadInstruction);
          }
        }

        // window memory
        if (value->isType(Fact::Type::Win) && !value->isLocked()) {
          auto path = value->getPath();
          auto type = loadInstruction->getPointerOperand()->getType();
          bool isPointerOfPointer = type->getContainedType(0)->isPointerTy();

          if (value->knowsAddress(loadInstruction->getPointerOperand())) {
            if (isPointerOfPointer) {
              value->addKnownAddress(loadInstruction);
            } else {
              path->transit(WindowState::State::LocalRead, loadInstruction, MPISymbol::MPILocalRead);
            }
          } else if (value->knowsLocalAddress(loadInstruction->getPointerOperand())) {
            if (isPointerOfPointer) {
              value->addKnownLocalAddress(loadInstruction, loadInstruction->getPointerOperand());
            } else {
              path->transit(WindowState::State::LocalRead, loadInstruction, MPISymbol::MPIDataRead);
            }
          } else if (value->knowsSharedAddress(loadInstruction->getPointerOperand())) {
            if (isPointerOfPointer) {
              value->addKnownSharedAddress(loadInstruction, loadInstruction->getPointerOperand());
            } else {
              const llvm::CallInst *generator = nullptr;
              auto &valueCallMap = value->getValueCallMap();
              auto searchResult = valueCallMap.find(loadInstruction->getPointerOperand());

              if (valueCallMap.end() != searchResult) {
                if (auto callInst = llvm::dyn_cast<llvm::CallInst>(searchResult->second)) {
                  generator = callInst;
                }
              }

              path->transit(
                  WindowState::State::RemoteCommunication
                , loadInstruction
                , MPISymbol::MPISharedRead
                , generator
              );
            }
          }
        }

        return {value};
      }
    };

    return std::make_shared<
        BranchPipe<LoadTracer>
      >(  loadInstruction
        , *this
        , loadInstruction
      );
  }

  if (auto GEPInstruction = llvm::dyn_cast<llvm::GetElementPtrInst>(CurrentInstruction)) {
    struct GEPTracer : psr::FlowFunction<D_Type*> {
      const llvm::GetElementPtrInst *instruction;

      GEPTracer (const llvm::GetElementPtrInst *L) : instruction(L) {}

      std::set<D_Type*> computeTargets (D_Type* value) {
        auto pointer = instruction->getPointerOperand();

        if ( value->isType(Fact::Type::Null)
          || value->isType(Fact::Type::Comm)
        ) {
          // Duplicate of block below
          if (value->knowsRankValue(pointer)) {
            const std::set<const llvm::GetElementPtrInst*> *offsetInfo = nullptr;

            if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(pointer)) {
              offsetInfo = value->findOffsetInfo(loadInstruction->getPointerOperand());
            } else {
              offsetInfo = value->findOffsetInfo(pointer);
            }

            if (nullptr != offsetInfo && isKnownOffset(instruction, offsetInfo)) {
              value->addKnownRankValue(instruction);
            }

            return {value};
          }
        }

        if ( value->isType(Fact::Type::Win)
          || value->isType(Fact::Type::Comm)
        ) {
          if (value->knowsValue(pointer)) {
            const std::set<const llvm::GetElementPtrInst*> *offsetInfo = nullptr;

            if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(pointer)) {
              offsetInfo = value->findOffsetInfo(loadInstruction->getPointerOperand());
            } else {
              offsetInfo = value->findOffsetInfo(pointer);
            }

            if (nullptr != offsetInfo && isKnownOffset(instruction, offsetInfo)) {
              value->addKnownValue(instruction);
            }

            return {value};
          }
        }

        if (value->isType(Fact::Type::Win)) {
          if (value->knowsAddress(pointer)) {
            // See Fact.cpp for details; in short: let's live without offsets.
            value->addKnownAddress(instruction);
          } else if (value->knowsSharedAddress(pointer)) {
            value->addKnownSharedAddress(instruction, pointer);
          } else if (value->knowsLocalAddress(pointer)) {
            value->addKnownLocalAddress(instruction, pointer);
          }
        }

        return {value};
      }
    };

    return std::make_shared<
        BranchPipe<GEPTracer>
      >(  GEPInstruction
        , *this
        , GEPInstruction
      );
  }

  return std::make_shared<
      BranchPipe<NonSingletonIdentity>
    >(
        CurrentInstruction
      , *this
    );
}

std::shared_ptr<psr::FlowFunction<MPIRMAUseProblem::D_Type*>>
MPIRMAUseProblem::getCallFlowFunction (
    const llvm::Instruction *callInstruction
  , const llvm::Function *targetFunction
) {
  if (  llvm::isa<llvm::CallInst>(callInstruction)
    ||  llvm::isa<llvm::InvokeInst>(callInstruction)
  ) {
    llvm::ImmutableCallSite callSite(callInstruction);

    if (!targetFunction->isDeclaration()) {
      struct CallTracer : psr::FlowFunction<D_Type*> {
        llvm::ImmutableCallSite callSite;
        const llvm::Function *targetFunction;
        std::vector<const llvm::Value*> actuals;
        std::vector<const llvm::Value*> formals;

        CallTracer (
            llvm::ImmutableCallSite CallSite
          , const llvm::Function *TargetFunction
        ) : callSite(CallSite)
          , targetFunction(TargetFunction)
        {
          for (unsigned idx = 0; idx < callSite.getNumArgOperands(); ++idx) {
            actuals.push_back(callSite.getArgOperand(idx));
          }

          for (unsigned idx = 0; idx < targetFunction->arg_size(); ++idx) {
            formals.push_back(psr::getNthFunctionArgument(targetFunction, idx));
          }
        }

        std::set<D_Type*> computeTargets (D_Type* value) {
          if ( value->isType(Fact::Type::Win)
            || value->isType(Fact::Type::Comm)
            || value->isType(Fact::Type::Null)
            || value->isType(Fact::Type::IntConst)
          ) {
            value->pushTokenLayers();

            for (unsigned idx = 0; idx < actuals.size(); ++idx) {
              if (value->knowsValue(actuals[idx])) {
                value->addKnownValue(formals[idx]);
              } else if (value->knowsAddress(actuals[idx])) {
                value->addKnownAddress(formals[idx]);
              } else if (value->knowsLocalAddress(actuals[idx])) {
                value->addKnownLocalAddress(formals[idx], actuals[idx]);
              } else if (value->knowsRankValue(actuals[idx])) {
                value->addKnownRankValue(formals[idx]);
              } else if (value->knowsSharedAddress(actuals[idx])) {
                value->addKnownSharedAddress(formals[idx], actuals[idx]);
              }
            }
          }

          return {value};
        }
      };

      return std::make_shared<
          BranchPipe<CallTracer>
        >(  callInstruction
          , *this
          , callSite
          , targetFunction
        );
    }
  }

  return std::make_shared<
      BranchPipe<NonSingletonIdentity>
    >(
        callInstruction
      , *this
    );
}

std::shared_ptr<psr::FlowFunction<MPIRMAUseProblem::D_Type*>>
MPIRMAUseProblem::getRetFlowFunction (
    const llvm::Instruction *callSite
  , const llvm::Function *calleeMethod
  , const llvm::Instruction *exitInstruction
  , const llvm::Instruction*
) {
  struct CallReturnTracer : psr::FlowFunction<D_Type*> {
    llvm::ImmutableCallSite callSite;
    const llvm::Function *calleeMethod;
    const llvm::ReturnInst *exitInstruction;
    std::vector<const llvm::Value*> actuals;
    std::vector<const llvm::Value*> formals;

    CallReturnTracer (
        llvm::ImmutableCallSite CallSite
      , const llvm::Function *CalleeMethod
      , const llvm::Instruction *ExitInstruction
    ) : callSite(CallSite)
      , calleeMethod(CalleeMethod)
      , exitInstruction(llvm::dyn_cast<llvm::ReturnInst>(ExitInstruction))
    {
      for (unsigned idx = 0; idx < callSite.getNumArgOperands(); ++idx) {
        actuals.push_back(callSite.getArgOperand(idx));
      }

      for (unsigned idx = 0; idx < calleeMethod->arg_size(); ++idx) {
        formals.push_back(psr::getNthFunctionArgument(calleeMethod, idx));
      }
    }

    std::set<D_Type*> computeTargets (D_Type* value) {
      if ( value->isType(Fact::Type::Win)
        || value->isType(Fact::Type::Comm)
        || value->isType(Fact::Type::Null)
        || value->isType(Fact::Type::IntConst)
      ) {
        std::set<const llvm::Value*> backports;
        std::set<const llvm::Value*> rankBackports;
        const llvm::Value *addressBackport = nullptr;
        const llvm::Value *localAddressBackport = nullptr;
        const llvm::Value *sharedAddressBackport = nullptr;

        // This is for writing values into pointers given in the call params.
        for (unsigned idx = 0; idx < formals.size(); ++idx) {
          if (formals[idx]->getType()->isPointerTy()) {
            if (value->knowsValue(formals[idx])) {
              backports.insert(actuals[idx]);
            } else if (value->knowsRankValue(formals[idx])) {
              rankBackports.insert(actuals[idx]);
            }
          }
          // pointers to pointers currently ignored (window memory)
        }

        if (value->knowsValue(exitInstruction->getReturnValue())) {
          backports.insert(callSite.getInstruction());
        } else if (value->knowsAddress(exitInstruction->getReturnValue())) {
          addressBackport = callSite.getInstruction();
        } else if (value->knowsLocalAddress(exitInstruction->getReturnValue())) {
          localAddressBackport = callSite.getInstruction();
        } else if (value->knowsRankValue(exitInstruction->getReturnValue())) {
          rankBackports.insert(callSite.getInstruction());
        } else if (value->knowsSharedAddress(exitInstruction->getReturnValue())) {
          sharedAddressBackport = callSite.getInstruction();
        }

        // Since this one will pass all returns eventually, we must do this only
        // at the CRF. Also: Do not do this earlier, since otherwise exit values
        // will be turn unknown.
        value->popTokenLayers();

        for (auto v : backports) { value->addKnownValue(v); }
        for (auto v : rankBackports) { value->addKnownRankValue(v); }
        if (nullptr != addressBackport) { value->addKnownAddress(addressBackport); }
        if (nullptr != sharedAddressBackport) {
          value->addKnownSharedAddress(sharedAddressBackport, exitInstruction->getReturnValue());
        }
        if (nullptr != localAddressBackport) {
          value->addKnownLocalAddress(localAddressBackport, exitInstruction->getReturnValue());
        }
      }

      return {value};
    }
  };

  return std::make_shared<
      BranchPipe<CallReturnTracer>
    >(  exitInstruction
      , *this
      , llvm::ImmutableCallSite(callSite)
      , calleeMethod
      , exitInstruction
    );
}

std::shared_ptr<psr::FlowFunction<MPIRMAUseProblem::D_Type*>>
MPIRMAUseProblem::getCallToRetFlowFunction (
    const llvm::Instruction *callSite
  , const llvm::Instruction *retSite
  , std::set<const llvm::Function*> callees
) {
  struct Terminator : psr::FlowFunction<D_Type*> {
    std::set<const llvm::Function*> functions;

    Terminator (std::set<const llvm::Function*> Functions)
      : functions(Functions)
    {}

    std::set<D_Type*> computeTargets (D_Type *Value) {
      // If this was Identity, the fact killing of the summary
      // goes unnoticed.
      if (Value->isMarkedForPurging()) {
        return {};
      } else {
        // For Null fact, try to make sure not to pop on things
        // that aren't defined in the module as Null contains rank
        // values that might get lost otherwise.
        if (Value->isType(Fact::Type::Null)) {
          bool safeDefinitions = true;

          // Properly handling multiple functions is not modeled yet!
          // In theory, that requires a clone-and-die fact per entering
          // of a function.
          for (auto function : functions) {
            if (function->isDeclaration()) {
              safeDefinitions = false;
              break;
            }
          }

          if (safeDefinitions) {
            Value->popTokenLayers();
          }
        }

        return {Value};
      }
    }
  };

  return std::make_shared<Terminator>(std::move(callees));
}

std::shared_ptr<psr::FlowFunction<MPIRMAUseProblem::D_Type*>>
MPIRMAUseProblem::getSummaryFlowFunction (
    const llvm::Instruction *callInstruction
  , const llvm::Function *targetFunction
) {
  // TODO: do some cleanup, CallSite->getInstruction() ends up empty
  // in some cases, so just work with `castedCallInstruction` in the
  // meantime.
  if (    llvm::isa<llvm::CallInst>(callInstruction)
      ||  llvm::isa<llvm::InvokeInst>(callInstruction)
  ) {
    llvm::ImmutableCallSite callSite(callInstruction);
    auto castedCallInstruction = llvm::dyn_cast<llvm::CallInst>(callInstruction);

    /**
     * For reasons of Fortran, when adding a new function, always check for
     * the most specific name prefix first!
     *
     * Also: Add any new function to the classification in
     * `BlockClassification.cpp`.
     */
    if (targetFunction->getName().startswith_lower("MPI_Accumulate")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  callInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIAccumulate
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Barrier")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  callInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIBarrier
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_accept")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(4));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_connect")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(4));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_create_group")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(3));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (
         targetFunction->getName().equals("MPI_Comm_create")
      || targetFunction->getName().equals_lower("MPI_Comm_create_")
    ) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(2));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_disconnect")) {
      // Missing atm: The handle becomes `MPI_COMM_WORLD` after that.
      return std::make_shared<
          BranchPipe<FactKiller>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , Fact::Type::Comm
          , callSite.getArgument(0)
          , castedCallInstruction
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_dup_with_info")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(2));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_dup")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(1));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_idup")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(1));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (
         targetFunction->getName().equals("MPI_Comm_free")
      || targetFunction->getName().equals_lower("MPI_Comm_free_")
    ) {
      return std::make_shared<
          BranchPipe<FactKiller>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , Fact::Type::Comm
          , callSite.getArgument(0)
          , castedCallInstruction
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_join")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(1));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_rank")) {
      struct RankTracker : psr::FlowFunction<D_Type*> {
        const llvm::CallInst *callInst;

        RankTracker (const llvm::CallInst *CallInst)
          : callInst(CallInst) {}

        std::set<D_Type*> computeTargets (D_Type *Value) {
          auto commValue = callInst->getArgOperand(0);
          auto rankValue = callInst->getArgOperand(1);
          auto constant = llvm::dyn_cast<llvm::ConstantInt>(commValue);

          // MPI_COMM_WORLD: assign to null
          if (constant) {
            if (Value->isType(Fact::Type::Null)) {
              Value->addKnownRankValue(rankValue, true);
            }
          // actual comm handles: assign rank there
          } else {
            if ( Value->isType(Fact::Type::Comm)
              && Value->knowsValue(commValue)
            ) {
              Value->addKnownRankValue(rankValue, true);
            }
          }

          return {Value};
        }
      };

      return std::make_shared<
          BranchPipe<RankTracker>
        >(  callInstruction
          , targetFunction
          , *this
          , castedCallInstruction
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_spawn_multiple")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(7));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_spawn")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(6));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_split_type")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(4));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_split")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(3));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Compare_and_swap")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  callInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPICompareAndSwap
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Finalize")) {
      struct MPITerminator : psr::FlowFunction<D_Type*> {
        std::set<D_Type*> computeTargets (D_Type *Value) {
          if (!Value->isType(Fact::Type::Null)) {
            return {};
          } else {
            return {Value};
          }
        }
      };

      return std::make_shared<MPITerminator>();
    } else if (targetFunction->getName().startswith_lower("MPI_Get_accumulate")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  callInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIGetAccumulate
        );
    } else if (
         targetFunction->getName().equals("MPI_Get")
      || targetFunction->getName().equals_lower("MPI_Get_")
    ) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  callInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIGet
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Fetch_and_op")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  callInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIFetchAndOp
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Intercomm_create")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(5));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Intercomm_merge")) {
      D_Type *fact =
        makeInitialFact(
            Fact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(2));

      return std::make_shared<
          BranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Put")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIPut
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Raccumulate")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIRaccumulate
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Rget_accumulate")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIRgetAccumulate
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Rget")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIRget
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Rput")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIRput
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_allocate_shared")) {
      return std::make_shared<
          BranchPipe<WindowCreator>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , callSite
          , *this
          , zeroValue()
          , MPISymbol::MPIWinAllocateShared
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_allocate")) {
      return std::make_shared<
          BranchPipe<WindowCreator>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , callSite
          , *this
          , zeroValue()
          , MPISymbol::MPIWinAllocate
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_attach")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinAttach
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_complete")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinComplete
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_create_dynamic")) {
      return std::make_shared<
          BranchPipe<WindowCreator>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , callSite
          , *this
          , zeroValue()
          , MPISymbol::MPIWinCreateDynamic
        );
    } else if (
         targetFunction->getName().equals("MPI_Win_create")
      || targetFunction->getName().equals_lower("MPI_Win_create_")
    ) {
      return std::make_shared<
          BranchPipe<WindowCreator>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , callSite
          , *this
          , zeroValue()
          , MPISymbol::MPIWinCreate
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_detach")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinDetach
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_fence")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFence
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_flush_all")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFlushAll
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_flush_local_all")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFlushLocalAll
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_flush_local")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFlushLocal
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_flush")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFlush
        );
    } else if (
         targetFunction->getName().equals("MPI_Win_free")
      || targetFunction->getName().equals_lower("MPI_Win_free_")
    ) {
      return std::make_shared<
          BranchPipe<FactKiller>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , Fact::Type::Win
          , callSite.getArgument(0)
          , castedCallInstruction
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_lock_all")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinLockAll
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_lock")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinLock
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_post")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinPost
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_shared_query")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinSharedQuery
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_start")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinStart
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_sync")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinSync
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_test")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinTest
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_unlock_all")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinUnlockAll
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_unlock")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinUnlock
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_wait")) {
      return std::make_shared<
          BranchPipe<SymbolicExecutor>
        >(  castedCallInstruction
          , targetFunction
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinWait
        );
    }

    // Cannot be nullptr in case of a BB's first instruction being
    // a call to something different (like LLVM debug things).
    auto callTargetFunction = castedCallInstruction->getCalledFunction();
    if (nullptr != callTargetFunction && callTargetFunction->isDeclaration()) {
      return std::make_shared<
          BranchPipe<NonSingletonIdentity>
        >(
            castedCallInstruction
          , targetFunction
          , *this
        );
    }
  }

  return nullptr;
}

std::map<const llvm::Instruction *, std::set<MPIRMAUseProblem::D_Type*>>
MPIRMAUseProblem::initialSeeds () {
  std::map<const llvm::Instruction *, std::set<D_Type*>> seedMap;

  for (auto &entryPoint : entryPoints) {
    seedMap.insert(
      std::make_pair(
          &icfg.getMethod(entryPoint)->front().front()
        , std::set<D_Type*>({zeroValue()})
      )
    );
  }

  return seedMap;
}

const MPIRMAUseProblem::FactsCollection& MPIRMAUseProblem::getFacts () const {
  return facts;
}

bool MPIRMAUseProblem::isKnownOffset (
    const llvm::GetElementPtrInst *instruction
  , const std::set<const llvm::GetElementPtrInst*> *candidates
) {
  if (!instruction->hasAllConstantIndices()) {
    return false;
  }

  for (auto it = candidates->begin(); it != candidates->end(); ++it) {
    auto candidateInstruction = *it;
    if (candidateInstruction->getNumIndices() != instruction->getNumIndices()) {
      continue;
    }

    const uint64_t numIndices = candidateInstruction->getNumIndices();

    bool mismatch = false;
    for (uint64_t i = 1; i < numIndices; ++i) {
      auto indexA = llvm::dyn_cast<llvm::ConstantInt>(candidateInstruction->getOperand(i));
      auto indexB = llvm::dyn_cast<llvm::ConstantInt>(instruction->getOperand(i));

      if (indexA->getZExtValue() != indexB->getZExtValue()) {
        mismatch = true;
        break;
      }
    }

    if (mismatch) {
      continue;
    } else {
      return true;
    }
  }

  return false;
}
