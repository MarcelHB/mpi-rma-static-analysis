#include <algorithm>
#include <stack>

#include "llvm/IR/Instructions.h"

#include "UnlockMapGenerator.h"

/**
 * In order to perform no-going-back semantics for loopish CFGs, we better
 * scout ahead in an offline mode where to start locking ICFG facts and
 * where to unlock.
 *
 * Here, we have three steps per function of the given module:
 *
 * 1. DFS over the BB-CFG, doing the following:
 *    * map our aux. data as graph over the existing one
 *    * detect edges going back (as in a loop)
 *
 * 2a. For each back-edge, do Tarjan starting from the back-edge destination
 *    BB, while blocking any other edge going there. This way, we obtain
 *    all strongly-connected components. So any SCC not being the starting
 *    one indicates a reachable exit of that loop.
 *
 * 2b. Over the SCC'd subgraph, do a DFS starting from the source of a back-
 *    edge. Stop on the occurrence of a node that belongs to another SCC,
 *    collect all exits and pick the one of the shortest distance of hops.
 *
 * 3. Create a map of branch => { type, lock edge, exit edge }.
 */

UnlockMapGenerator::UnlockMapGenerator (psr::ProjectIRDB &DB)
  : projectIRDB(DB)
{}

std::pair<UnlockMap, UnlockMap> UnlockMapGenerator::createMap () {
  UnlockMap map, directMap;

  for (auto function : projectIRDB.getAllFunctions()) {
    UnlockMapGenerator::FunctionContext context;

    initialDiscovery(function, context);

    for (auto &pair : context.blockMap) {
      auto &bbProps = pair.second;

      if (bbProps.firstSuccIsBackEdge || bbProps.secondSuccIsBackEdge) {
        context.reset();
        tarjanDiscovery(bbProps, context);
        exitDiscovery(bbProps, context);
      }
    }

    fillUnlockMap(map, directMap, context);
  }

  return std::make_pair(map, directMap);
}

void UnlockMapGenerator::fillUnlockMap (
    UnlockMap &map
  , UnlockMap &directMap
  , FunctionContext &context
) const {
  for (auto &pair : context.blockMap) {
    auto &bbProps = pair.second;

    if (nullptr != bbProps.exitProps) {
      auto terminator = bbProps.block.getTerminator();
      auto nextTerminator = bbProps.exitProps->block.getTerminator();

      const llvm::Instruction *exit = nullptr, *destination = nullptr, *nextDestination = nullptr;
      auto exitProps = bbProps.exitProps;

      if (bbProps.firstSuccIsBackEdge) {
        destination = &(bbProps.firstSuccProps->block.front());
      } else if (bbProps.secondSuccIsBackEdge) {
        destination = &(bbProps.secondSuccProps->block.front());
      }

      if (0 == exitProps->exitBranchIdx) {
        exit = &(exitProps->firstSuccProps->block.front());
        nextDestination = &(exitProps->secondSuccProps->block.front());
      } else {
        exit = &(exitProps->secondSuccProps->block.front());
        nextDestination = &(exitProps->firstSuccProps->block.front());
      }

      map.emplace(
          std::piecewise_construct
        , std::forward_as_tuple(terminator)
        , std::forward_as_tuple(destination, exit, UnlockMapOperation::UnlockMapType::BackEdge)
      );

      /**
       * This is for the lazy approach that defers the whole thing by one node. That's better anyway
       * so the upper thing should be removed eventually and work like this one: If there is an MPI
       * call in the loop head, it gets lost in the upper one.
       */
      UnlockMapOperation::UnlockMapType type = UnlockMapOperation::UnlockMapType::ForwardInDo;

      /**
       * A bit of heuristics: If the BB following the back edge is the same as the exit, but not
       * the same as the current BB, treat as a while-loop. Otherwise, it's do-while, and we have
       * already entered the loop.
       */
      if ( (   bbProps.firstSuccIsBackEdge
            && &(bbProps.firstSuccProps->block) == &(exitProps->block)
            && &(bbProps.firstSuccProps->block) != &(bbProps.block)
           )
        || (   bbProps.secondSuccIsBackEdge
            && &(bbProps.secondSuccProps->block) == &(exitProps->block)
            && &(bbProps.secondSuccProps->block) != &(bbProps.block)
           )
      ) {
        type = UnlockMapOperation::UnlockMapType::Forward;
      }

      directMap.emplace(
          std::piecewise_construct
        , std::forward_as_tuple(nextTerminator)
        , std::forward_as_tuple(nextDestination, exit, type)
      );
    }
  }
}

void UnlockMapGenerator::exitDiscovery (
    BasicBlockProperties &bbProps
  , FunctionContext &context
) const {

  auto exitSuggestion = visitExit(bbProps, context, 0);
  auto branchProps = std::get<1>(exitSuggestion);
  auto exitSuccIdx = std::get<2>(exitSuggestion);

  if (nullptr != branchProps) {
    bbProps.exitProps = branchProps;
    branchProps->exitBranchIdx = exitSuccIdx;
  }
}

UnlockMapGenerator::ExitSuggestion UnlockMapGenerator::visitExit (
    BasicBlockProperties &bbProps
  , FunctionContext &context
  , uint64_t i
) const {
  UnlockMapGenerator::ExitSuggestion exit{i, nullptr, 0};
  bbProps.exitDiscoveryColor = context.exitDiscoveryGray;

  if (nullptr != bbProps.firstSuccProps) {
    if (context.exitDiscoveryGray > bbProps.firstSuccProps->exitDiscoveryColor) {
      if (bbProps.firstSuccProps->sccNum == bbProps.sccNum) {
        exit = visitExit(*(bbProps.firstSuccProps), context, i+1);
      } else {
        exit = UnlockMapGenerator::ExitSuggestion{i+1, &bbProps, 0};
      }
    } else if(context.exitDiscoveryGray < bbProps.firstSuccProps->exitDiscoveryColor) {
      if (bbProps.firstSuccProps->sccNum != bbProps.sccNum) {
        exit = UnlockMapGenerator::ExitSuggestion{i+1, &bbProps, 0};
      }
    }
  }

  UnlockMapGenerator::ExitSuggestion otherExit{i, nullptr, 1};
  if (nullptr != bbProps.secondSuccProps) {
    if (context.exitDiscoveryGray > bbProps.secondSuccProps->exitDiscoveryColor) {
      if (bbProps.secondSuccProps->sccNum == bbProps.sccNum) {
        otherExit = visitExit(*(bbProps.secondSuccProps), context, i+1);
      } else {
        otherExit = UnlockMapGenerator::ExitSuggestion{i+1, &bbProps, 1};
      }
    } else if(context.exitDiscoveryGray < bbProps.secondSuccProps->exitDiscoveryColor) {
      if (bbProps.secondSuccProps->sccNum != bbProps.sccNum) {
        otherExit = UnlockMapGenerator::ExitSuggestion{i+1, &bbProps, 1};
      }
    }
  }

  bbProps.exitDiscoveryColor = context.exitDiscoveryGray + 1;

  if (nullptr != std::get<1>(otherExit)) {
    if (nullptr == std::get<1>(exit) || std::get<0>(otherExit) < std::get<0>(exit)) {
      return otherExit;
    }
  }

  return exit;
}

void UnlockMapGenerator::initialDiscovery (
    const llvm::Function *function
  , UnlockMapGenerator::FunctionContext &context
) const {
  for (auto &bb : function->getBasicBlockList()) {
    context.blockMap.emplace(
        std::piecewise_construct
      , std::forward_as_tuple(&bb)
      , std::forward_as_tuple(bb)
    );
  }

  auto &firstBB = function->getEntryBlock();
  auto searchResult = context.blockMap.find(&firstBB);

  if (context.blockMap.end() != searchResult) {
    auto &firstBBProps = searchResult->second;
    visit(firstBBProps, context);
  }
}

void UnlockMapGenerator::tarjanDiscovery (
    UnlockMapGenerator::BasicBlockProperties &bbProps
  , UnlockMapGenerator::FunctionContext &context
) const {
  context.fromBB = &bbProps;

  if (bbProps.firstSuccIsBackEdge) {
    context.bannedBB = bbProps.firstSuccProps;
  } else if (bbProps.secondSuccIsBackEdge) {
    context.bannedBB = bbProps.secondSuccProps;
  }

  visitTarjan(*(context.bannedBB), context);
}

void UnlockMapGenerator::visit (
    UnlockMapGenerator::BasicBlockProperties &bbProps
  , FunctionContext &context
) const {
  auto termInstruction = bbProps.block.getTerminator();
  bbProps.color = UnlockMapGenerator::DiscoveryColor::Gray;

  if (auto branchInstruction = llvm::dyn_cast<llvm::BranchInst>(termInstruction)) {
    visitChild(branchInstruction, 0, bbProps, context);

    if (branchInstruction->isConditional()) {
      visitChild(branchInstruction, 1, bbProps, context);
    }
  }

  bbProps.color = context.discoveredColor;
}

void UnlockMapGenerator::visitTarjan (
    UnlockMapGenerator::BasicBlockProperties &bbProps
  , FunctionContext &context
) const {
  context.ticks += 1;

  bbProps.discoveryTick = context.ticks;
  bbProps.lowPointTick = context.ticks;
  bbProps.stacked = true;
  context.bbPropsStack.push(&bbProps);

  if (nullptr != bbProps.firstSuccProps) {
    visitChildTarjan(*(bbProps.firstSuccProps), bbProps, context);
  }

  if (nullptr != bbProps.secondSuccProps) {
    visitChildTarjan(*(bbProps.secondSuccProps), bbProps, context);
  }

  if (bbProps.discoveryTick == bbProps.lowPointTick) {
    auto otherBBProps = context.bbPropsStack.top();
    do {
      otherBBProps = context.bbPropsStack.top();
      context.bbPropsStack.pop();

      otherBBProps->stacked = false;
      otherBBProps->sccNum = context.currentSCCNum;
    } while(otherBBProps != &bbProps);

    context.currentSCCNum += 1;
  }

  context.ticks += 1;
  bbProps.finishTick = context.ticks;
}

void UnlockMapGenerator::visitChild (
    const llvm::BranchInst *branchInst
  , unsigned i
  , BasicBlockProperties &bbProps
  , FunctionContext &context
) const {
  auto successorBB = branchInst->getSuccessor(i);

  if (nullptr != successorBB) {
    auto searchResult = context.blockMap.find(successorBB);

    if (context.blockMap.end() != searchResult) {
      auto &succBBProps = searchResult->second;

      if (0 == i) {
        bbProps.firstSuccProps = &succBBProps;
      } else {
        bbProps.secondSuccProps = &succBBProps;
      }

      if (context.undiscoveredColor == succBBProps.color) {
        visit(succBBProps, context);
      } else if (UnlockMapGenerator::DiscoveryColor::Gray == succBBProps.color) {
        if (0 == i) {
          bbProps.firstSuccIsBackEdge = true;
        } else {
          bbProps.secondSuccIsBackEdge = true;
        }
      }
    }
  }
}

void UnlockMapGenerator::visitChildTarjan (
    BasicBlockProperties &succBBProps
  , BasicBlockProperties &bbProps
  , FunctionContext &context
) const {
  if (context.bannedBB == &succBBProps && context.fromBB != &bbProps) {
    return;
  }

  if (context.baseTick >= succBBProps.discoveryTick) {
    visitTarjan(succBBProps, context);

    bbProps.lowPointTick = std::min(bbProps.lowPointTick, succBBProps.lowPointTick);
  } else if (succBBProps.discoveryTick < bbProps.lowPointTick) {
    if (context.baseTick >= succBBProps.finishTick) {
      bbProps.lowPointTick = succBBProps.discoveryTick;
    } else if (succBBProps.finishTick > context.baseTick && succBBProps.stacked) {
      bbProps.lowPointTick = succBBProps.discoveryTick;
    }
  }
}
