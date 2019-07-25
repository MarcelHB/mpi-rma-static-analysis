#ifndef H_MOTF_RESOLVER
#define H_MOTF_RESOLVER

#include <llvm/IR/DerivedTypes.h>

#include <phasar/DB/ProjectIRDB.h>
#include <phasar/PhasarLLVM/ControlFlow/Resolver/OTFResolver.h>

/**
 * Essentially an OTFResolver, but also looking what's behind a `bitcast`
 * in case of a function pointer, as we otherwise don't find calls
 * like `mpi_win_create_` generated by Flang.
 */

struct MOTFResolver : public psr::OTFResolver {
  public:
    MOTFResolver (
        psr::ProjectIRDB &irdb
      , psr::LLVMTypeHierarchy &ch
      , psr::PointsToGraph &wholemodulePTG
    );
    virtual ~MOTFResolver () = default;

    const llvm::Function* getFunctionFromCast (const llvm::ImmutableCallSite&);

    std::set<std::string> resolveFunctionPointer (const llvm::ImmutableCallSite &CS) override;

  private:
    psr::ProjectIRDB &IRDB;
};

#endif
