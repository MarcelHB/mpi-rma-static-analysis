#ifndef H_RANK_ATTRIBUTION_AND_CONTRADICION_CHECK
#define H_RANK_ATTRIBUTION_AND_CONTRADICION_CHECK

#include "Fact.h"

class RankAttributionAndContradictionCheck {
  public:
    RankAttributionAndContradictionCheck (Fact*);

    const llvm::Instruction* attributeAndValidate ();

  private:
    Fact *window;

    const llvm::Value* getRankValueFromCondition(const Fact*, const llvm::CmpInst*) const;
};

#endif
