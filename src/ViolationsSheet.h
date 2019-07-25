#ifndef H_VIOLATIONS_SHEET
#define H_VIOLATIONS_SHEET

#include <ostream>

#include "llvm/IR/Instruction.h"

#include "Fact.h"

class ViolationsSheet {
  public:
    ViolationsSheet (std::ostream&);

    void writeHeaderRow ();
    void writeViolation (
        const Fact*
      , const llvm::Instruction*
      , const std::string&
    );

    static const char colSep = ',';

  private:
    std::ostream& outStream;
};

#endif
