#ifndef H_PATH_MERGER
#define H_PATH_MERGER

#include <map>
#include <set>

#include "boost/graph/adjacency_list.hpp"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"

#include "Settings.h"

class WormholeMapGenerator {
  public:
    struct WormholeEntry {
      bool leftRight;
      const llvm::BasicBlock *left;
      const llvm::BasicBlock *right;

      WormholeEntry (
          bool LeftRight
        , const llvm::BasicBlock *Left
        , const llvm::BasicBlock *Right
      ) : leftRight(LeftRight)
        , left(Left)
        , right(Right)
      {}
    };

    struct OperationWormholeEntry {
      bool leftRight;
      llvm::BasicBlock *left;
      llvm::BasicBlock *right;

      OperationWormholeEntry (const WormholeEntry &WE)
        : leftRight(WE.leftRight)
        , left(const_cast<llvm::BasicBlock*>(WE.left))
        , right(const_cast<llvm::BasicBlock*>(WE.right))
      {}
    };

    using WormholeMap =
      std::map<
          const llvm::BasicBlock*
        , WormholeEntry
      >;
    using OperationWormholeMap =
      std::map<
          llvm::BasicBlock*
        , OperationWormholeEntry
      >;

    WormholeMapGenerator (const llvm::Module&, const Settings&);

    WormholeMap createWormholeMap ();
  private:
    enum class Color { SuperEnd, Regular, Interesting, Branch, BranchInteresting };
    enum class DiscoveryColor { White, Gray, Black };

    struct VertexProperties;

    using Graph = boost::adjacency_list<
        boost::setS
      , boost::vecS
      , boost::bidirectionalS
      , VertexProperties*
    >;

    using GraphVD = boost::graph_traits<Graph>::vertex_descriptor;

    struct BranchVisitInfo {
      bool isGray;
      bool isRightDescendant;
      bool mustDiscard;

      GraphVD dominantDescriptor;
      GraphVD belongsToBranchDescriptor;
      GraphVD interestingBranchDescriptor;

      Color dominatorColor;

      BranchVisitInfo ()
        : isGray(false)
        , isRightDescendant(false)
        , mustDiscard(false)
        , dominatorColor(Color::Regular)
      {}

      BranchVisitInfo (GraphVD VD, Color C)
        : isGray(false)
        , isRightDescendant(false)
        , mustDiscard(false)
        , dominantDescriptor(VD)
        , belongsToBranchDescriptor(VD)
        , dominatorColor(C)
      {}

      BranchVisitInfo (GraphVD VD, Color C, GraphVD BranchVD, bool IsRight)
        : isGray(false)
        , isRightDescendant(IsRight)
        , mustDiscard(false)
        , dominantDescriptor(VD)
        , belongsToBranchDescriptor(VD)
        , interestingBranchDescriptor(BranchVD)
        , dominatorColor(C)
      {}

      BranchVisitInfo (const BranchVisitInfo &Other)
        : isGray(Other.isGray)
        , mustDiscard(Other.mustDiscard)
        , dominantDescriptor(Other.dominantDescriptor)
        , dominatorColor(Other.dominatorColor)
      {}

      void subordinate (GraphVD DomDesc, GraphVD Desc) {
        dominantDescriptor = DomDesc;
        dominatorColor = Color::Branch;
        belongsToBranchDescriptor = Desc;
      }
    };

    struct VisitInfo {
      bool isClear;
      bool isGray;
      GraphVD branchDescriptor;
      GraphVD belongsToBranchDescriptor;
      std::set<GraphVD> unresolvedSymbols;

      VisitInfo ()
        : isClear(true)
        , isGray(false)
      {}

      VisitInfo (GraphVD VD)
        : isClear(false)
        , isGray(false)
        , branchDescriptor(VD)
        , belongsToBranchDescriptor(VD)
      {
        unresolvedSymbols.insert(VD);
      }

      VisitInfo (const VisitInfo& Other)
        : isClear(Other.isClear)
        , isGray(Other.isGray)
        , branchDescriptor(Other.branchDescriptor)
      {
        if (!Other.isClear) {
          unresolvedSymbols.insert(Other.belongsToBranchDescriptor);
        }
      }

      void subordinate (GraphVD Desc) {
        belongsToBranchDescriptor = Desc;
        unresolvedSymbols.insert(Desc);
      }
    };

    struct VertexProperties {
      const llvm::BasicBlock *block;
      Color color;
      DiscoveryColor discoveryColor;
      VisitInfo returnedVisitInfo;
      BranchVisitInfo returnedBranchVisitInfo;
      GraphVD descriptor;
      bool closesLoop;

      VertexProperties (const llvm::BasicBlock *Block)
        : block(Block)
        , color(Color::Regular)
        , discoveryColor(DiscoveryColor::White)
        , closesLoop(false)
      {}

      VertexProperties ()
        : block(nullptr)
        , color(Color::SuperEnd)
        , discoveryColor(DiscoveryColor::White)
        , closesLoop(false)
      {}

      VertexProperties (const VertexProperties &Other)
        : block(Other.block)
        , color(Other.color)
        , discoveryColor(Other.discoveryColor)
        , descriptor(Other.descriptor)
        , closesLoop(Other.closesLoop)
      {}
    };

    struct FunctionContext {
      Graph graph;
      std::set<GraphVD> exitBlocks;
      std::map<const llvm::BasicBlock*, VertexProperties> blockMap;
      std::map<
          const llvm::BasicBlock*
        , WormholeEntry
      > blockWormholes;
      DiscoveryColor undiscoveredColor;
      DiscoveryColor discoveredColor;
      VertexProperties endNode;
      GraphVD endNodeDescriptor;

      FunctionContext ()
        : undiscoveredColor(DiscoveryColor::White)
        , discoveredColor(DiscoveryColor::Black)
      {}

      void reset () {
        auto oldColor = undiscoveredColor;
        undiscoveredColor = discoveredColor;
        discoveredColor = oldColor;
      }
    };

    const llvm::Module &module;
    const Settings &settings;

    void appendSuperEnd (FunctionContext&) const;
    void buildGraph (const llvm::Function*, FunctionContext&) const;
    void buildWormholeMap (const FunctionContext&, WormholeMap&) const;
    bool isComingOverRightEdge (GraphVD, GraphVD, const FunctionContext&) const;
    void shrinkGraph (FunctionContext&) const;

    GraphVD visit (
        VertexProperties&
      , FunctionContext&
      , GraphVD*
    ) const;

    void visitChild (
        const llvm::BranchInst*
      , unsigned i
      , VertexProperties&
      , FunctionContext&
      , GraphVD
    ) const;

    BranchVisitInfo& visitBranchShrinking (GraphVD, FunctionContext&) const;
    VisitInfo& visitShrinking (GraphVD, FunctionContext&) const;
};

#endif
