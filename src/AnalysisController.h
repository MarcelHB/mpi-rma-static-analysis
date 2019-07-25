#ifndef H_ANALYSIS_CONTROLLER
#define H_ANALYSIS_CONTROLLER

#include <ostream>

#include "Settings.h"
#include "MPIRMAUseProblem.h"

class AnalysisController {
  public:
    AnalysisController (
        const MPIRMAUseProblem::FactsCollection&
      , const Settings&
    );
    void run () const;

  private:
    const MPIRMAUseProblem::FactsCollection& facts;
    const Settings &settings;
};

#endif
