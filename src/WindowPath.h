#ifndef H_RMA_STATE_MACHINE
#define H_RMA_STATE_MACHINE

#include <list>
#include <tuple>

#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstrTypes.h"

#include "MPISymbols.h"
#include "WindowState.h"

class WindowPath {
  public:
    struct Transition {
      const llvm::Instruction *instruction;
      const llvm::CallInst *generator;
      WindowState::State state;
      MPISymbol symbol;

      Transition (
          WindowState::State S
        , const llvm::Instruction *Instruction
        , MPISymbol Sym
        , const llvm::CallInst *Generator
      ) : instruction(Instruction)
        , generator(Generator)
        , state(S)
        , symbol(Sym)
      {}
    };

    using Transitions = std::list<Transition>;

    WindowPath ();
    WindowPath (const WindowPath&);

    const WindowPath* getPreviousPath () const;
    llvm::CmpInst::Predicate getRankPredicate () const;
    int64_t getRankValue () const;
    const llvm::Instruction* getViolatingInstruction () const;
    const Transitions& getTransitions () const;

    bool hasFinalState () const;
    bool hasRankPredicate () const;

    void setRankAttributes (llvm::CmpInst::Predicate, int64_t);
    void setViolatingInstruction (const llvm::Instruction*);

    void transit (
        WindowState::State
      , const llvm::Instruction*
      , MPISymbol
      , const llvm::CallInst *Generator = nullptr
    );

  private:
    const WindowPath * const previousPath;
    const llvm::Instruction *violatingInstruction;
    WindowState state;

    Transitions transitions;
    llvm::CmpInst::Predicate rankPredicate;
    int64_t rankConstantValue;
};

#endif
