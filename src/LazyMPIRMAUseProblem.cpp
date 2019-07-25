#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"

#include "phasar/PhasarLLVM/IfdsIde/FlowFunctions/Gen.h"
#include "phasar/PhasarLLVM/IfdsIde/FlowFunctions/Identity.h"
#include "phasar/PhasarLLVM/IfdsIde/FlowFunctions/Kill.h"
#include "phasar/PhasarLLVM/Plugins/Interfaces/IfdsIde/IFDSTabulationProblemPlugin.h"
#include "phasar/Utils/LLVMShorthands.h"

#include "LazyBranchPipe.h"
#include "LazyFactKiller.h"
#include "LazyMPIRMAUseProblem.h"
#include "LazySymbolicExecutor.h"
#include "LazyWindowCreator.h"
#include "MPISymbols.h"

LazyMPIRMAUseProblem::LazyMPIRMAUseProblem (
    psr::LLVMBasedICFG &ICFG
  , const UnlockMap &UnlockMap
  , std::vector<std::string> &EntryPoints
  , const Settings &Settings
) : LazyMPIRMAIFDSClass(ICFG)
  , entryPoints(EntryPoints)
  , _zeroValue(std::make_unique<D_Type>(LazyFact::Type::Null, UnlockMap, Settings.ithWindow))
  , unlockMap(UnlockMap)
{
  DefaultIFDSTabulationProblem::zerovalue = createZeroValue();
}

LazyMPIRMAUseProblem::D_Type* LazyMPIRMAUseProblem::createZeroValue () {
  return _zeroValue.get();
}

std::map<const llvm::Instruction *, std::set<LazyMPIRMAUseProblem::D_Type*>>
LazyMPIRMAUseProblem::initialSeeds () {
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

std::shared_ptr<psr::FlowFunction<LazyMPIRMAUseProblem::D_Type*>>
LazyMPIRMAUseProblem::getNormalFlowFunction (
    const llvm::Instruction *CurrentInstruction
  , const llvm::Instruction *SuccessorInstruction
) {
  if (auto bitcastInstruction = llvm::dyn_cast<llvm::BitCastInst>(CurrentInstruction)) {
    struct BitcastTracer : psr::FlowFunction<D_Type*> {
      const llvm::BitCastInst *bcInst;

      BitcastTracer (
          const llvm::BitCastInst *BC
      ) : bcInst(BC)
      {}

      std::set<D_Type*> computeTargets (D_Type* value) {
        if ( value->isType(LazyFact::Type::Win)
          || value->isType(LazyFact::Type::Comm)
        ) {
          if (value->knowsValue(bcInst->getOperand(0))) {
            value->addKnownValue(bcInst);
          }
        }

        return {value};
      }
    };

    return std::make_shared<
        LazyBranchPipe<BitcastTracer>
      >(  bitcastInstruction
        , SuccessorInstruction
        , *this
        , bitcastInstruction
      );
  }

  if (auto storeInstruction = llvm::dyn_cast<llvm::StoreInst>(CurrentInstruction)) {
    struct StoreTracer : psr::FlowFunction<D_Type*> {
      const llvm::StoreInst *storeInstruction;

      StoreTracer (const llvm::StoreInst *S) : storeInstruction(S) {}

      std::set<D_Type*> computeTargets (D_Type* value) {
        if ( value->isType(LazyFact::Type::Comm)
          || value->isType(LazyFact::Type::Win)
        ) {
          if (value->knowsValue(storeInstruction->getValueOperand())) {
            value->addKnownValue(storeInstruction->getPointerOperand());
          }
        }

        return {value};
      }
    };

    return std::make_shared<
        LazyBranchPipe<StoreTracer>
      >(  storeInstruction
        , SuccessorInstruction
        , *this
        , storeInstruction
      );
  }

  if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(CurrentInstruction)) {
    struct LoadTracer : psr::FlowFunction<D_Type*> {
      const llvm::LoadInst *loadInstruction;

      LoadTracer (const llvm::LoadInst *L) : loadInstruction(L) {}

      std::set<D_Type*> computeTargets (D_Type* value) {
        if ( value->isType(LazyFact::Type::Win)
          || value->isType(LazyFact::Type::Comm)
        ) {
          if (value->knowsValue(loadInstruction->getPointerOperand())) {
            value->addKnownValue(loadInstruction);
          }
        }

        return {value};
      }
    };

    return std::make_shared<
        LazyBranchPipe<LoadTracer>
      >(  loadInstruction
        , SuccessorInstruction
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

        if ( value->isType(LazyFact::Type::Win)
          || value->isType(LazyFact::Type::Comm)
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

        return {value};
      }
    };

    return std::make_shared<
        LazyBranchPipe<GEPTracer>
      >(  GEPInstruction
        , SuccessorInstruction
        , *this
        , GEPInstruction
      );
  }

  return std::make_shared<
      LazyBranchPipe<NonSingletonIdentity>
    >(
        CurrentInstruction
      , SuccessorInstruction
      , *this
    );
}

std::shared_ptr<psr::FlowFunction<LazyMPIRMAUseProblem::D_Type*>>
LazyMPIRMAUseProblem::getCallFlowFunction (
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
          if ( value->isType(LazyFact::Type::Win)
            || value->isType(LazyFact::Type::Comm)
            || value->isType(LazyFact::Type::Null)
          ) {
            value->pushTokenLayers();

            for (unsigned idx = 0; idx < actuals.size(); ++idx) {
              if (value->knowsValue(actuals[idx])) {
                value->addKnownValue(formals[idx]);
              }
            }
          }

          return {value};
        }
      };

      return std::make_shared<
          LazyBranchPipe<CallTracer>
        >(  callInstruction
          , nullptr
          , *this
          , callSite
          , targetFunction
        );
    }
  }

  return std::make_shared<
      LazyBranchPipe<NonSingletonIdentity>
    >(
        callInstruction
      , nullptr
      , *this
    );
}

std::shared_ptr<psr::FlowFunction<LazyMPIRMAUseProblem::D_Type*>>
LazyMPIRMAUseProblem::getRetFlowFunction (
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
      if ( value->isType(LazyFact::Type::Win)
        || value->isType(LazyFact::Type::Comm)
        || value->isType(LazyFact::Type::Null)
      ) {
        std::set<const llvm::Value*> backports;

        for (unsigned idx = 0; idx < formals.size(); ++idx) {
          if (formals[idx]->getType()->isPointerTy()) {
            if (value->knowsValue(formals[idx])) {
              backports.insert(actuals[idx]);
            }
          }
        }

        if (value->knowsValue(exitInstruction->getReturnValue())) {
          backports.insert(callSite.getInstruction());
        }

        value->popTokenLayers();

        for (auto v : backports) {
          value->addKnownValue(v);
        }
      }

      return {value};
    }
  };

  return std::make_shared<
      LazyBranchPipe<CallReturnTracer>
    >(  exitInstruction
      , nullptr
      , *this
      , llvm::ImmutableCallSite(callSite)
      , calleeMethod
      , exitInstruction
    );
}

std::shared_ptr<psr::FlowFunction<LazyMPIRMAUseProblem::D_Type*>>
LazyMPIRMAUseProblem::getCallToRetFlowFunction (
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
      if (Value->isMarkedForPurging()) {
        return {};
      } else {
        return {Value};
      }
    }
  };

  return std::make_shared<Terminator>(std::move(callees));
}

std::shared_ptr<psr::FlowFunction<LazyMPIRMAUseProblem::D_Type*>>
LazyMPIRMAUseProblem::getSummaryFlowFunction (
    const llvm::Instruction *callInstruction
  , const llvm::Function *targetFunction
) {
  if (    llvm::isa<llvm::CallInst>(callInstruction)
      ||  llvm::isa<llvm::InvokeInst>(callInstruction)
  ) {
    llvm::ImmutableCallSite callSite(callInstruction);
    auto castedCallInstruction = llvm::dyn_cast<llvm::CallInst>(callInstruction);

    if (targetFunction->getName().startswith_lower("MPI_Accumulate")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  callInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIAccumulate
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_accept")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(4));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_connect")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(4));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_create_group")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(3));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
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
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(2));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_disconnect")) {
      return std::make_shared<
          LazyBranchPipe<LazyFactKiller>
        >(  castedCallInstruction
          , nullptr
          , *this
          , LazyFact::Type::Comm
          , callSite.getArgument(0)
          , castedCallInstruction
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_dup_with_info")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(2));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_dup")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(1));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_idup")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(1));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (
         targetFunction->getName().equals("MPI_Comm_free")
      || targetFunction->getName().equals_lower("MPI_Comm_free_")
    ) {
      return std::make_shared<
          LazyBranchPipe<LazyFactKiller>
        >(  castedCallInstruction
          , nullptr
          , *this
          , LazyFact::Type::Comm
          , callSite.getArgument(0)
          , castedCallInstruction
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_join")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(1));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_spawn_multiple")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(7));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_spawn")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(6));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_split_type")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(4));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Comm_split")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(3));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Compare_and_swap")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  callInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPICompareAndSwap
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Finalize")) {
      struct MPITerminator : psr::FlowFunction<D_Type*> {
        std::set<D_Type*> computeTargets (D_Type *Value) {
          if (!Value->isType(LazyFact::Type::Null)) {
            return {};
          } else {
            return {Value};
          }
        }
      };

      return std::make_shared<MPITerminator>();
    } else if (targetFunction->getName().startswith_lower("MPI_Get_accumulate")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  callInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIGetAccumulate
          , *this
        );
    } else if (
         targetFunction->getName().equals("MPI_Get")
      || targetFunction->getName().equals_lower("MPI_Get_")
    ) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  callInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIGet
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Fetch_and_op")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  callInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIFetchAndOp
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Intercomm_create")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(5));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Intercomm_merge")) {
      D_Type *fact =
        makeInitialFact(
            LazyFact::Type::Comm
          , zeroValue()
        );
      fact->addKnownValue(callSite.getArgument(2));

      return std::make_shared<
          LazyBranchPipe<psr::Gen<D_Type*>>
        >(  castedCallInstruction
          , nullptr
          , *this
          , fact
          , zeroValue()
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Put")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIPut
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Raccumulate")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIRaccumulate
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Rget_accumulate")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIRgetAccumulate
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Rget")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIRget
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Rput")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIRput
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_allocate_shared")) {
      return std::make_shared<
          LazyBranchPipe<LazyWindowCreator>
        >(  castedCallInstruction
          , nullptr
          , *this
          , callSite
          , *this
          , zeroValue()
          , MPISymbol::MPIWinAllocateShared
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_allocate")) {
      return std::make_shared<
          LazyBranchPipe<LazyWindowCreator>
        >(  castedCallInstruction
          , nullptr
          , *this
          , callSite
          , *this
          , zeroValue()
          , MPISymbol::MPIWinAllocate
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_complete")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinComplete
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_create_dynamic")) {
      return std::make_shared<
          LazyBranchPipe<LazyWindowCreator>
        >(  castedCallInstruction
          , nullptr
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
          LazyBranchPipe<LazyWindowCreator>
        >(  castedCallInstruction
          , nullptr
          , *this
          , callSite
          , *this
          , zeroValue()
          , MPISymbol::MPIWinCreate
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_fence")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFence
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_flush_all")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFlushAll
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_flush_local_all")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFlushLocalAll
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_flush_local")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFlushLocal
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_flush")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinFlush
          , *this
        );
    } else if (
         targetFunction->getName().equals("MPI_Win_free")
      || targetFunction->getName().equals_lower("MPI_Win_free_")
    ) {
      return std::make_shared<
          LazyBranchPipe<LazyFactKiller>
        >(  castedCallInstruction
          , nullptr
          , *this
          , LazyFact::Type::Win
          , callSite.getArgument(0)
          , castedCallInstruction
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_lock_all")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinLockAll
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_lock")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinLock
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_post")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinPost
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_start")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinStart
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_sync")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinSync
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_test")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinTest
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_unlock_all")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinUnlockAll
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_unlock")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinUnlock
          , *this
        );
    } else if (targetFunction->getName().startswith_lower("MPI_Win_wait")) {
      return std::make_shared<
          LazyBranchPipe<LazySymbolicExecutor>
        >(  castedCallInstruction
          , nullptr
          , *this
          , castedCallInstruction
          , MPISymbol::MPIWinWait
          , *this
        );
    }

    auto callTargetFunction = castedCallInstruction->getCalledFunction();
    if (nullptr != callTargetFunction && callTargetFunction->isDeclaration()) {
      return std::make_shared<
          LazyBranchPipe<NonSingletonIdentity>
        >(
            castedCallInstruction
          , nullptr
          , *this
        );
    }
  }

  return nullptr;
}

const LazyMPIRMAUseProblem::FactsCollection& LazyMPIRMAUseProblem::getFacts () const {
  return facts;
}

bool LazyMPIRMAUseProblem::isLoopHead (
    const llvm::BranchInst *BranchInst
  , UnlockMapOperation::UnlockMapType *type
) const {
  auto searchResult = unlockMap.find(BranchInst);

  if (searchResult != unlockMap.end()) {
    *type = searchResult->second.type;

    return true;
  }

  return false;
}

bool LazyMPIRMAUseProblem::isKnownOffset (
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
