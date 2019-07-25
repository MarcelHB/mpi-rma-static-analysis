#ifndef H_CONCURRENT_COMM_CHECK
#define H_CONCURRENT_COMM_CHECK

#include "EpochMatcher.h"
#include "MPISymbols.h"

class ConcurrentCommCheck {
  public:
    ConcurrentCommCheck (EpochMatcher::WindowGuides&);

    static unsigned getRankArgPosition (MPISymbol);

    std::map<Fact*, const llvm::Instruction*> validate () const;

  private:
    EpochMatcher::WindowGuides &guides;

    int64_t getRankValue (Fact*, const EpochMatcher::MatchedInstruction&) const;
    std::set<Fact*> getTargetWindows (int64_t, const EpochMatcher::MatchedInstruction&) const;
};

#endif
