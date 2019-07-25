#ifndef H_EPOCH_MATCHER
#define H_EPOCH_MATCHER

#include <list>
#include <map>
#include <set>
#include <vector>

#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstrTypes.h"

#include "Fact.h"
#include "MPISymbols.h"

class EpochMatcher {
  public:
    enum class Epoch {
        Passive
      , Locking
      , LockingAll
      , Expose
      , Access
      , ExposeAccess
      , Fence
    };

    enum class Violation {
        None
      , NotAllAvailable
      , Unmatched
    };

    struct Instruction {
      const llvm::Instruction *instruction;
      const llvm::CallInst *generator;
      MPISymbol symbol;

      Instruction ()
        : instruction(nullptr)
        , generator(nullptr)
        , symbol(MPISymbol::Null)
      {}

      Instruction (
          const llvm::Instruction *Instr
        , MPISymbol Symbol
        , const llvm::CallInst *Generator = nullptr
      )
        : instruction(Instr)
        , generator(Generator)
        , symbol(Symbol)
      {}

      Instruction (const Instruction &Other)
        : instruction(Other.instruction)
        , generator(Other.generator)
        , symbol(Other.symbol)
      {}
    };

    struct MatchedInstruction {
      Instruction instruction;
      Epoch epoch;
      std::map<Fact*, std::list<Instruction>> matchingInstructions;

      MatchedInstruction () : epoch(Epoch::Passive) {}

      MatchedInstruction (
          const llvm::Instruction *Instr
        , MPISymbol Symbol
        , Epoch Epoch
        , const llvm::CallInst *Generator = nullptr
      ) : instruction(std::move(Instruction{Instr, Symbol, Generator}))
        , epoch(Epoch)
      {}
    };

    struct WindowEpochGuide {
      Fact *window;
      const llvm::Instruction *violatingInstruction;
      std::vector<MatchedInstruction> instructions;
      int64_t rankValue;
      llvm::CmpInst::Predicate rankPredicate;
      Violation violation;

      WindowEpochGuide ()
        : window(nullptr)
        , violatingInstruction(nullptr)
        , rankValue(-1)
        , rankPredicate(llvm::CmpInst::Predicate::BAD_ICMP_PREDICATE)
        , violation(Violation::None)
      {}

      WindowEpochGuide (
          Fact *Window
        , WindowPath *path
      ) : window(Window)
        , violatingInstruction(nullptr)
        , rankValue(path->getRankValue())
        , rankPredicate(path->getRankPredicate())
        , violation(Violation::None)
      {}
    };

    using WindowGuides = std::map<Fact*, WindowEpochGuide>;

    EpochMatcher (std::set<Fact*>&);

    WindowGuides match () const;

  private:
    struct Context {
      WindowGuides guides;
    };

    std::set<Fact*> &facts;

    void coExecute (Context&) const;
    MPISymbol getBlockingInstruction (Instruction&) const;
    void linearize (Context&) const;
    void validateMatching (Context&) const;
};

#endif
