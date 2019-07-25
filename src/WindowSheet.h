#ifndef H_WINDOW_SHEET
#define H_WINDOW_SHEET

#include <ostream>

#include "llvm/IR/Instructions.h"

#include "Fact.h"

class WindowSheet {
  public:
    WindowSheet (std::ostream&);

    void writeHeaderRow ();
    void writeWindowFact (const Fact*);

    static const char colSep = ',';
  private:
    std::ostream& outStream;

    std::string getRankPredicateString (llvm::CmpInst::Predicate) const;
};

#endif
