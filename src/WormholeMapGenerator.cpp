#include <algorithm>

#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"

#include "BlockClassification.h"
#include "WormholeMapGenerator.h"

/**
 * Wormholes: because we want to prevent creating forks for nothing,
 * as growth is exponentially.
 *
 * 1. In a DFS, mark each BB as interesting if it looks MPI-ish or
 *   contains store of integer constants. Also, mark branch nodes,
 *   and node can be both, interesting and branches.
 *
 * 2. In a second DFS, starting from a super-end node, we try to find
 *   dominating branch nodes, i.e. they are not subordinate to a higher
 *   one. Dominating branch nodes disappear when going through an
 *   interesting node, otherwise ending in some merge node. If a
 *   merge node acquires the same domination-info from all other
 *   nodes upwards, the CFG it *may* become the other end of the wormhole
 *   to the propagated dominating branch node. But only if every branch
 *   has truly merged into this node, i.e. not having other paths that
 *   will merge at a later point.
 *
 *   Examples:
 *
 *                (dom. branch node - A)
 *       /                /   \
 *       |               /\   /\
 *     (flow)              ...
 *       \                \   /
 *                   (merge node - B)
 *
 *   We want a wormhole from A to B, whatever the flow between looks
 *   like - it just doesn't have anything interesting. Above of A, there is
 *   either the CFG's root, or some other interesting node that broke
 *   dominance.
 *
 *   To achieve that, we use a quite simple method:
 *
 *   1. For any branch, it passes its symbol down to both its direct children,
 *     let's say "A" is obtained by C_1 and C_2. C_2 again has children
 *     D_1, D_2. While both of them receive "C_2", only one of them also gets
 *     "A" (or any other grand-x-parent). Otherwise, we'd duplicate "A"
 *     to infinity - we want to have it exactly twice.
 *
 *   2. For each merging node, collect the symbols obtained by each node
 *     above, and do a pair-wise symm. set differnce. If two symbols have arrived
 *     at the same node, they are resovled. And only B is the node that
 *     eventually resolves the last pair of symbols.
 *
 *   This is embedded inside the 2nd DFS; the set difference is O(N) and occurs
 *   in up to the same number of branch nodes (n/2), thus it's O(N^2) in total.
 *
 * 3. In a third DFS, we look for CFG structures that closely look like:
 *
 *   if (a && b && c) {
 *     MPI_...();
 *   }
 *
 *  or
 *
 *   if (a || b || c) {
 *     MPI_...();
 *   }
 *
 *   In order to strip down n direct conjunctive or disjunctive conditions right
 *   before an interesting MPI block into one branch, instead of n. The algorithm
 *   is quite similar to 2.: again, we look for a dominating branch, and if we
 *   end up in the merge node with exactly one dominator and one interesting
 *   predecessor, we have either structure. This does not cover anything less
 *   trivial than that right now!
 */

WormholeMapGenerator::WormholeMapGenerator (
    const llvm::Module &Module
  , const Settings &S
)
  : module(Module)
  , settings(S)
{}

void WormholeMapGenerator::appendSuperEnd (FunctionContext &context) const {
  auto descriptor = boost::add_vertex(&(context.endNode), context.graph);

  for (auto exitDesc : context.exitBlocks) {
    boost::add_edge(exitDesc, descriptor, context.graph);
  }

  context.endNodeDescriptor = descriptor;
}

void WormholeMapGenerator::buildGraph (
    const llvm::Function *function
  , FunctionContext &context
) const {
  for (auto &bb : function->getBasicBlockList()) {
    context.blockMap.emplace(
        std::piecewise_construct
      , std::forward_as_tuple(&bb)
      , std::forward_as_tuple(&bb)
    );
  }

  auto &firstBB = function->getEntryBlock();
  auto searchResult = context.blockMap.find(&firstBB);

  if (context.blockMap.end() != searchResult) {
    auto &firstBBProps = searchResult->second;

    visit(firstBBProps, context, nullptr);
    appendSuperEnd(context);
    context.reset();
    shrinkGraph(context);
  }
}

void WormholeMapGenerator::buildWormholeMap (
    const FunctionContext &context
  , WormholeMap &wormholeMap
) const {
  wormholeMap.insert(
      std::make_move_iterator(context.blockWormholes.begin())
    , std::make_move_iterator(context.blockWormholes.end())
  );
}

WormholeMapGenerator::WormholeMap WormholeMapGenerator::createWormholeMap () {
  WormholeMap map;

  for (auto &function : module.functions()) {
    WormholeMapGenerator::FunctionContext context;

    buildGraph(&function, context);
    buildWormholeMap(context, map);
  }

  return map;
}

void WormholeMapGenerator::shrinkGraph (FunctionContext &context) const {
  auto range = boost::in_edges(context.endNodeDescriptor, context.graph);

  for (auto it = range.first; it != range.second; ++it) {
    visitShrinking(boost::source(*it, context.graph), context);
  }

  context.reset();

  for (auto it = range.first; it != range.second; ++it) {
    visitBranchShrinking(boost::source(*it, context.graph), context);
  }
}

WormholeMapGenerator::GraphVD WormholeMapGenerator::visit (
    VertexProperties &vertProps
  , FunctionContext &context
  , GraphVD *parentVertDescriptor
) const {
  auto termInstruction = vertProps.block->getTerminator();
  auto descriptor = boost::add_vertex(&vertProps, context.graph);

  vertProps.discoveryColor = WormholeMapGenerator::DiscoveryColor::Gray;
  vertProps.descriptor = descriptor;

  bool isBranch = false;
  if (auto branchInstruction = llvm::dyn_cast<llvm::BranchInst>(termInstruction)) {
    visitChild(branchInstruction, 0, vertProps, context, descriptor);

    if (branchInstruction->isConditional()) {
      isBranch = true;
      visitChild(branchInstruction, 1, vertProps, context, descriptor);
    }
  } else if (auto retInstruction = llvm::dyn_cast<llvm::ReturnInst>(termInstruction)) {
    context.exitBlocks.insert(descriptor);
  }

  if (shouldPreserveBlock(*(vertProps.block), settings)) {
    vertProps.color = WormholeMapGenerator::Color::Interesting;
  }

  if (isBranch && !vertProps.closesLoop) {
    if (WormholeMapGenerator::Color::Interesting == vertProps.color) {
      vertProps.color = WormholeMapGenerator::Color::BranchInteresting;
    } else {
      vertProps.color = WormholeMapGenerator::Color::Branch;
    }
  }

  vertProps.discoveryColor = context.discoveredColor;

  return descriptor;
}

void WormholeMapGenerator::visitChild (
    const llvm::BranchInst *branchInst
  , unsigned i
  , VertexProperties &vertProps
  , FunctionContext &context
  , GraphVD parentVertDescriptor
) const {
  auto successorBB = branchInst->getSuccessor(i);

  if (nullptr != successorBB) {
    auto searchResult = context.blockMap.find(successorBB);

    if (context.blockMap.end() != searchResult) {
      auto &succBBProps = searchResult->second;
      GraphVD childDescriptor;

      if (context.undiscoveredColor == succBBProps.discoveryColor) {
        childDescriptor = visit(succBBProps, context, &parentVertDescriptor);
      } else {
        // Treat loop-heads/feet as regular nodes since they solve themself.
        if (WormholeMapGenerator::DiscoveryColor::Gray == succBBProps.discoveryColor) {
          succBBProps.closesLoop = true;
        }

        childDescriptor = succBBProps.descriptor;
      }

      boost::add_edge(parentVertDescriptor, childDescriptor, context.graph);
    }
  }
}

WormholeMapGenerator::BranchVisitInfo& WormholeMapGenerator::visitBranchShrinking (
    GraphVD currentVertexDescriptor
  , FunctionContext &context
) const {
  VertexProperties *props = context.graph[currentVertexDescriptor];
  props->discoveryColor = WormholeMapGenerator::DiscoveryColor::Gray;

  auto range = boost::in_edges(currentVertexDescriptor, context.graph);
  bool hasBranchReturn = false
    , hasInterestingReturn = false
    , hasSubordinateBranchReturn = false
    , discard = false
    , branchDominatorFromRight = false;

  uint64_t ingoings = 0;
  GraphVD branchReturn, interestingReturn;

  for (auto it = range.first; it != range.second; ++it) {
    auto otherDesc = boost::source(*it, context.graph);
    auto otherProps = context.graph[otherDesc];
    BranchVisitInfo childVisitInfo;

    if (context.undiscoveredColor == otherProps->discoveryColor) {
      childVisitInfo = visitBranchShrinking(otherDesc, context);
    } else if (context.discoveredColor == otherProps->discoveryColor) {
      childVisitInfo = std::move(BranchVisitInfo{otherProps->returnedBranchVisitInfo});
    }

    if (WormholeMapGenerator::DiscoveryColor::Gray != otherProps->discoveryColor) {
      if (childVisitInfo.isGray) {
        if (childVisitInfo.dominantDescriptor == currentVertexDescriptor) {
          discard = true;
          continue;
        } else {
          props->returnedBranchVisitInfo = childVisitInfo;
          props->discoveryColor = context.discoveredColor;

          return props->returnedBranchVisitInfo;
        }
      }
    } else {
      props->returnedBranchVisitInfo.dominantDescriptor = otherProps->descriptor;
      props->returnedBranchVisitInfo.isGray = true;
      props->discoveryColor = context.discoveredColor;

      return props->returnedBranchVisitInfo;
    }

    ingoings += 1;

    if (childVisitInfo.mustDiscard) {
      discard = true;
    }

    if ( WormholeMapGenerator::Color::Branch == childVisitInfo.dominatorColor
      || WormholeMapGenerator::Color::BranchInteresting == childVisitInfo.dominatorColor
    ) {
      if (hasBranchReturn && branchReturn != childVisitInfo.dominantDescriptor) {
        discard = true;
      } else {
        hasBranchReturn = true;
        branchDominatorFromRight = isComingOverRightEdge(
            currentVertexDescriptor
          , childVisitInfo.dominantDescriptor
          , context
        );
        branchReturn = childVisitInfo.dominantDescriptor;

        if (childVisitInfo.dominantDescriptor != childVisitInfo.belongsToBranchDescriptor) {
          hasSubordinateBranchReturn = true;
        }
      }
    } else if (WormholeMapGenerator::Color::Interesting == childVisitInfo.dominatorColor) {
      if (hasInterestingReturn && interestingReturn != childVisitInfo.dominantDescriptor) {
        discard = true;
      } else {
        hasInterestingReturn = true;
        interestingReturn = childVisitInfo.dominantDescriptor;
      }
    }
  }

  bool sufficientlyLargeCut = (ingoings >= 2 && hasSubordinateBranchReturn) || ingoings > 2;

  if (discard) {
    if ( WormholeMapGenerator::Color::BranchInteresting == props->color
      || WormholeMapGenerator::Color::Branch == props->color
    ) {
      props->returnedBranchVisitInfo = BranchVisitInfo{currentVertexDescriptor, props->color};
    } else {
      props->returnedBranchVisitInfo.mustDiscard = true;
    }
  } else {
    if (sufficientlyLargeCut && hasBranchReturn && hasInterestingReturn) {
      auto propsC = context.graph[interestingReturn];
      VertexProperties *propsB = nullptr;
      auto propsA = context.graph[propsC->returnedBranchVisitInfo.interestingBranchDescriptor];

      bool toRight = propsC->returnedBranchVisitInfo.isRightDescendant;

      if (ingoings == 2) {
        toRight = !toRight;
        propsB = props;
      } else {
        propsB = propsC;
      }

      if (auto terminator = llvm::dyn_cast<llvm::BranchInst>(propsA->block->getTerminator())) {
        if (toRight) {
          context.blockWormholes.emplace(
              std::piecewise_construct
            , std::forward_as_tuple(propsA->block)
            , std::forward_as_tuple(true, nullptr, propsB->block)
          );
        } else {
          context.blockWormholes.emplace(
              std::piecewise_construct
            , std::forward_as_tuple(propsA->block)
            , std::forward_as_tuple(true, propsB->block, nullptr)
          );
        }

        hasBranchReturn = false;
        hasInterestingReturn = false;
      }
    }

    if (!hasBranchReturn && !hasInterestingReturn) {
      if ( WormholeMapGenerator::Color::BranchInteresting == props->color
        || WormholeMapGenerator::Color::Branch == props->color
      ) {
        props->returnedBranchVisitInfo = BranchVisitInfo{currentVertexDescriptor, props->color};
      } else {
        props->returnedBranchVisitInfo.mustDiscard = true;
      }
    } else if (hasBranchReturn && !hasInterestingReturn) {
      if (WormholeMapGenerator::Color::Branch == props->color) {
        props->returnedBranchVisitInfo.subordinate(branchReturn, currentVertexDescriptor);
      } else if (WormholeMapGenerator::Color::Regular == props->color) {
        props->returnedBranchVisitInfo = BranchVisitInfo{branchReturn, WormholeMapGenerator::Color::Branch};
      } else if (WormholeMapGenerator::Color::Interesting == props->color) {
        props->returnedBranchVisitInfo =
          BranchVisitInfo{
              currentVertexDescriptor
            , props->color
            , branchReturn
            , branchDominatorFromRight
        };
      } else if (WormholeMapGenerator::Color::BranchInteresting == props->color) {
        props->returnedBranchVisitInfo = BranchVisitInfo{currentVertexDescriptor, props->color};
      }
    } else if (!hasBranchReturn && hasInterestingReturn) {
      if (WormholeMapGenerator::Color::Regular == props->color) {
        props->returnedBranchVisitInfo = BranchVisitInfo{interestingReturn, WormholeMapGenerator::Color::Interesting};
      } else {
        props->returnedBranchVisitInfo.mustDiscard = true;
      }
    }
  }

  props->discoveryColor = context.discoveredColor;

  return props->returnedBranchVisitInfo;
}

bool WormholeMapGenerator::isComingOverRightEdge (
    GraphVD currentNode
  , GraphVD returnedNode
  , const FunctionContext &context
) const {
  auto currentProps = context.graph[currentNode];
  auto returnedProps = context.graph[returnedNode];

  if (auto terminatorInst = llvm::dyn_cast<llvm::BranchInst>(returnedProps->block->getTerminator())) {
    return terminatorInst->getSuccessor(1) == currentProps->block;
  }

  return false;
}

WormholeMapGenerator::VisitInfo& WormholeMapGenerator::visitShrinking (
    GraphVD currentVertexDescriptor
  , FunctionContext &context
) const {
  VertexProperties *props = context.graph[currentVertexDescriptor];
  props->discoveryColor = WormholeMapGenerator::DiscoveryColor::Gray;

  auto range = boost::in_edges(currentVertexDescriptor, context.graph);

  bool hasFirstChild = false
    , homogeneousReturns = true;
  uint64_t ingoings = 0;
  VisitInfo visitInfo;
  std::set<GraphVD> runningSet;

  auto &startInst = props->block->front();

  for (auto it = range.first; it != range.second; ++it) {
    auto otherDesc = boost::source(*it, context.graph);
    auto otherProps = context.graph[otherDesc];
    VisitInfo childVisitInfo;

    if (context.undiscoveredColor == otherProps->discoveryColor) {
      childVisitInfo = visitShrinking(otherDesc, context);
    } else if (context.discoveredColor == otherProps->discoveryColor) {
      childVisitInfo = std::move(VisitInfo{otherProps->returnedVisitInfo});
    }

    if (WormholeMapGenerator::DiscoveryColor::Gray != otherProps->discoveryColor) {
      if (childVisitInfo.isGray) {
        if (childVisitInfo.branchDescriptor == currentVertexDescriptor) {
          continue;
        } else {
          props->returnedVisitInfo = childVisitInfo;
          props->discoveryColor = context.discoveredColor;

          return props->returnedVisitInfo;
        }
      }

      ingoings += 1;

      if (!hasFirstChild) {
        hasFirstChild = true;
        visitInfo = childVisitInfo;

        runningSet.insert(
            childVisitInfo.unresolvedSymbols.begin()
          , childVisitInfo.unresolvedSymbols.end()
        );
      } else {
        if ( visitInfo.isClear != childVisitInfo.isClear
          || visitInfo.branchDescriptor != childVisitInfo.branchDescriptor
        ) {
          homogeneousReturns = false;
        }

        if (homogeneousReturns) {
          std::list<GraphVD> tempDiff;
          std::set_symmetric_difference(
              childVisitInfo.unresolvedSymbols.begin()
            , childVisitInfo.unresolvedSymbols.end()
            , runningSet.begin()
            , runningSet.end()
            , std::back_inserter(tempDiff)
          );

          runningSet.clear();
          runningSet.insert(
              std::make_move_iterator(tempDiff.begin())
            , std::make_move_iterator(tempDiff.end())
          );
        }
      }
    } else {
      props->returnedVisitInfo.branchDescriptor = otherProps->descriptor;
      props->returnedVisitInfo.isGray = true;
      props->discoveryColor = context.discoveredColor;

      return props->returnedVisitInfo;
    }
  }

  bool madeZero = false;
  if (homogeneousReturns && ingoings > 1) {
    madeZero = runningSet.size() == 0;
  }

  visitInfo.unresolvedSymbols.clear();
  if (homogeneousReturns) {
    visitInfo.unresolvedSymbols.insert(
        std::make_move_iterator(runningSet.begin())
      , std::make_move_iterator(runningSet.end())
    );
  }

  /**
   * Interesting branches are both disrupting and replacing, since we cannot
   * wormhole through them but after them.
   */
  if (WormholeMapGenerator::Color::BranchInteresting == props->color) {
    props->returnedVisitInfo = VisitInfo{currentVertexDescriptor};
  } else if (WormholeMapGenerator::Color::Branch == props->color) {
    /**
     * Take new branch superiority for a.) being the first (cleared) one or b.) it's
     * closing at the same time.
     */
    if (!homogeneousReturns || 0 == ingoings || visitInfo.isClear || madeZero) {
      props->returnedVisitInfo = VisitInfo{currentVertexDescriptor};
    } else if (homogeneousReturns && ingoings >= 1 && !visitInfo.isClear && !madeZero) {
      props->returnedVisitInfo = visitInfo;
      props->returnedVisitInfo.subordinate(currentVertexDescriptor);
    }
  } else if (WormholeMapGenerator::Color::Regular == props->color) {
    // Pass-through of whatever was there before, if it's been the same from all inputs.
    if (homogeneousReturns && ingoings >= 1 && !madeZero) {
      props->returnedVisitInfo = visitInfo;
    }
  }

  if (madeZero && !visitInfo.isClear) {
    auto propsDominator = context.graph[visitInfo.branchDescriptor];

    context.blockWormholes.emplace(
        std::piecewise_construct
      , std::forward_as_tuple(propsDominator->block)
      , std::forward_as_tuple(false, props->block, nullptr)
    );
  }

  props->discoveryColor = context.discoveredColor;

  return props->returnedVisitInfo;
}
