#ifndef H_STATISTICS_SHEET
#define H_STATISTICS_SHEET

#include <ostream>

#include "StatisticsFact.h"

class StatisticsSheet {
  public:
    StatisticsSheet (std::ostream&);

    void writeHeaderRow ();
    void writeStatistitcs (const StatisticsFact::Record&);

    static const char colSep = ',';
  private:
    std::ostream& outStream;
};

#endif
