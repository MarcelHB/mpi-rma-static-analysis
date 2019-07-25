#ifndef H_RANK_CONTRADICTION_RESOLVER
#define H_RANK_CONTRADICTION_RESOLVER

#include <list>
#include <utility>

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"

#include "Fact.h"
#include "WindowPath.h"

class RankContradictionResolver {
  public:
    struct Condition {
      llvm::CmpInst::Predicate pred;
      int64_t constantValue;

      Condition ()
        : pred(llvm::CmpInst::Predicate::BAD_ICMP_PREDICATE)
        , constantValue(0)
      {}

      Condition (llvm::CmpInst::Predicate Predicate, int64_t Value)
        : pred(Predicate)
        , constantValue(Value)
      {}
    };

    struct ResolvedCondition {
      const llvm::ICmpInst *instruction;
      Condition condition;

      ResolvedCondition () : instruction(nullptr) {}

      ResolvedCondition (
          const llvm::ICmpInst *Instruction
        , Condition &Cond
      ) : instruction(Instruction)
        , condition(Cond)
      {}

      ResolvedCondition (
          const llvm::ICmpInst *Instruction
        , Condition &&Cond
      ) : instruction(Instruction)
        , condition(Cond)
      {}
    };

    RankContradictionResolver (
        const ResolvedCondition&
      , const ResolvedCondition&
    );

    const ResolvedCondition getMoreSpecificCondition () const;
    bool isContradiction () const;

    static Condition getPositiveCondition (
        const llvm::ICmpInst*
      , const llvm::Value*
      , const std::list<Fact::IntegerValueSnapshot>*
      , WindowState::State
    );
  private:
    const ResolvedCondition &condition;
    const ResolvedCondition &otherCondition;
};

#endif
