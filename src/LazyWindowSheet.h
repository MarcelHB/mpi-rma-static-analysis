#ifndef H_LAZY_WINDOW_SHEET
#define H_LAZY_WINDOW_SHEET

#include <ostream>

#include "llvm/IR/Instructions.h"

#include "LazyFact.h"

class LazyWindowSheet {
  public:
    LazyWindowSheet (std::ostream&);

    void writeHeaderRow ();
    void writeWindowFact (const LazyFact*);

    static const char colSep = ',';
  private:
    std::ostream& outStream;
};

#endif
