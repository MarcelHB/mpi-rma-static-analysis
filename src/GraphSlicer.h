#ifndef H_GRAPH_SLICER
#define H_GRAPH_SLICER

// TODO: extract data structures
#include "WormholeMapGenerator.h"

class GraphSlicer {
  public:
    GraphSlicer (const WormholeMapGenerator::OperationWormholeMap&);

    void slice () const;
  private:
    const WormholeMapGenerator::OperationWormholeMap &wormholeMap;
};

#endif
