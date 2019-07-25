#include "StatisticsSheet.h"

StatisticsSheet::StatisticsSheet (std::ostream &OutStream)
  : outStream(OutStream)
{}

void StatisticsSheet::writeHeaderRow () {
  outStream << "num_instructions" << StatisticsSheet::colSep
    << "num_instructions_total" << StatisticsSheet::colSep
    << "num_branches" << StatisticsSheet::colSep
    << "num_windows" << StatisticsSheet::colSep
    << "num_integers"
    << std::endl;
}

void StatisticsSheet::writeStatistitcs (const StatisticsFact::Record &Record) {
  outStream << Record.numInstructions << StatisticsSheet::colSep
    << Record.numInstructionsTotal << StatisticsSheet::colSep
    << Record.numBranches << StatisticsSheet::colSep
    << Record.numWindows << StatisticsSheet::colSep
    << Record.numIntegers
    << std::endl;
}
