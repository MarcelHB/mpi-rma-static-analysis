#include "WindowModeCheck.h"

WindowModeCheck::WindowModeCheck (
    const std::set<Fact*> &Windows
) : windows(Windows)
{}

bool WindowModeCheck::validate () const {
  MPISymbol creatorSymbol = MPISymbol::Null;

  for (auto window : windows) {
    MPISymbol currentSymbol = window->getCreatorSymbol();

    switch (currentSymbol) {
      case MPISymbol::MPIWinAllocate:
      case MPISymbol::MPIWinCreate:
      case MPISymbol::MPIWinCreateDynamic:
        // simply merging for reducing to two modes
        currentSymbol = MPISymbol::MPIWinCreate;
        break;
      // that's MPISymbol::MPIWinAllocateShared
      default:
        break;
    }

    if (MPISymbol::Null == creatorSymbol) {
      creatorSymbol = currentSymbol;
    } else if (creatorSymbol != currentSymbol) {
      return false;
    }
  }

  return true;
}
