#ifndef H_SETTINGS
#define H_SETTINGS

#include <string>

struct Settings {
  enum class FriendlyExit { None, Help, Version };
  enum class Statistics { Regular, Only, Skip };
  enum class IntegerMode { All, Static, None };
  enum class IFDSMode { Full, Lazy };

  std::string entryPoint;
  std::string moduleFile;
  std::string pathPrefix;
  bool findWormholes;
  bool showPhasarDetails;
  bool tickWindowProcess;
  FriendlyExit friendlyExit;
  Statistics statisticsMode;
  IntegerMode integerMode;
  uint32_t benchmarkMode;
  IFDSMode ifdsMode;
  uint32_t ithWindow;

  Settings ()
    : findWormholes(false)
    , showPhasarDetails(false)
    , tickWindowProcess(false)
    , friendlyExit(FriendlyExit::None)
    , statisticsMode(Statistics::Regular)
    , integerMode(IntegerMode::All)
    , benchmarkMode(0)
    , ifdsMode(IFDSMode::Full)
    , ithWindow(0)
  {}

  bool wantsHelpfulExit () const { return friendlyExit != FriendlyExit::None; }
};

#endif
