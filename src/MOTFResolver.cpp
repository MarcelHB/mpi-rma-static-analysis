#include "llvm/IR/CallSite.h"

#include "phasar/Utils/LLVMShorthands.h"
#include "phasar/Utils/Logger.h"

#include "MOTFResolver.h"

MOTFResolver::MOTFResolver (
    psr::ProjectIRDB &irdb
  , psr::LLVMTypeHierarchy &ch
  , psr::PointsToGraph &wholemodulePTG
) : psr::OTFResolver(irdb, ch, wholemodulePTG)
  , IRDB(irdb)
{}

namespace bl = boost::log;

std::set<std::string>
MOTFResolver::resolveFunctionPointer (const llvm::ImmutableCallSite &CS) {
  auto &lg = psr::lg::get();
  LOG_IF_ENABLE(BOOST_LOG_SEV(lg, psr::DEBUG)
                << "Call function pointer: "
                << psr::llvmIRToString(CS.getInstruction()));

  std::set<std::string> possible_call_targets;

  if ( CS.getCalledValue() != nullptr
    && CS.getCalledValue()->getType()->isPointerTy()
  ) {
    /**
     * This is where we test for calls behind `bitcast` as we see them multiple
     * times in Flang code. Or worse indirections.
     */
    auto strippedValue = CS.getCalledValue()->stripPointerCasts();

    if (auto function = llvm::dyn_cast<llvm::Function>(strippedValue)) {
      return {function->getName().str()};
    }

    if (const llvm::FunctionType *ftype = llvm::dyn_cast<llvm::FunctionType>(
      CS.getCalledValue()->getType()->getPointerElementType()
    )) {
      for (auto f : IRDB.getAllFunctions()) {
        if (psr::matchesSignature(f, ftype)) {
          possible_call_targets.insert(f->getName().str());
        }
      }
    }
  }

  return possible_call_targets;
}

const llvm::Function* MOTFResolver::getFunctionFromCast (const llvm::ImmutableCallSite &CS) {
  if (auto bitcast = llvm::dyn_cast<llvm::BitCastInst>(CS.getCalledValue())) {
    return llvm::dyn_cast<llvm::Function>(bitcast->getOperand(0));
  }

  return nullptr;
}

