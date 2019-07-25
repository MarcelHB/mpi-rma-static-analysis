#ifndef H_UNLOCK_MAP_GENERATOR
#define H_UNLOCK_MAP_GENERATOR

#include <map>
#include <stack>
#include <tuple>

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"

#include "phasar/DB/ProjectIRDB.h"

#include "UnlockMap.h"

/**
 * const'ing things had to respect laziness of ProjectIRDB.
 */
class UnlockMapGenerator {
  public:
    UnlockMapGenerator (psr::ProjectIRDB&);

    std::pair<UnlockMap, UnlockMap> createMap ();

  private:
    enum class DiscoveryColor { White, Gray, Black };

    struct BasicBlockProperties {
      BasicBlockProperties *firstSuccProps;
      BasicBlockProperties *secondSuccProps;
      BasicBlockProperties *exitProps;
      DiscoveryColor color;
      const llvm::BasicBlock &block;
      uint64_t sccNum;
      uint64_t discoveryTick;
      uint64_t finishTick;
      uint64_t lowPointTick;
      uint64_t exitBranchIdx;
      bool stacked;
      bool firstSuccIsBackEdge;
      bool secondSuccIsBackEdge;
      uint64_t exitDiscoveryColor;

      BasicBlockProperties (const llvm::BasicBlock &BB)
        : firstSuccProps(nullptr)
        , secondSuccProps(nullptr)
        , exitProps(nullptr)
        , color(DiscoveryColor::White)
        , block(BB)
        , sccNum(0)
        , discoveryTick(0)
        , finishTick(0)
        , lowPointTick(0)
        , exitBranchIdx(0)
        , stacked(false)
        , firstSuccIsBackEdge(false)
        , secondSuccIsBackEdge(false)
        , exitDiscoveryColor(0)
      {}
    };

    using ExitSuggestion =
      std::tuple<uint64_t, BasicBlockProperties*, unsigned long>;

    struct FunctionContext {
      BasicBlockProperties *bannedBB;
      BasicBlockProperties *fromBB;
      std::map<const llvm::BasicBlock*, BasicBlockProperties> blockMap;
      std::stack<BasicBlockProperties*> bbPropsStack;
      uint64_t ticks;
      uint64_t baseTick;
      uint64_t currentSCCNum;
      DiscoveryColor undiscoveredColor;
      DiscoveryColor discoveredColor;
      uint64_t exitDiscoveryGray;

      FunctionContext ()
        : bannedBB(nullptr)
        , fromBB(nullptr)
        , ticks(0)
        , baseTick(0)
        , undiscoveredColor(DiscoveryColor::White)
        , discoveredColor(DiscoveryColor::Black)
        , exitDiscoveryGray(1)
      {}

      void reset () {
        currentSCCNum = 0;
        bannedBB = fromBB = nullptr;
        // This spares us from resetting each bbProp after each DFS.
        baseTick = ticks;
        auto oldColor = undiscoveredColor;
        undiscoveredColor = discoveredColor;
        discoveredColor = oldColor;
        exitDiscoveryGray += 2;
      }
    };

    psr::ProjectIRDB &projectIRDB;

    void fillUnlockMap (UnlockMap&, UnlockMap&, FunctionContext&) const;

    void exitDiscovery (BasicBlockProperties&, FunctionContext&) const;
    void initialDiscovery (const llvm::Function*, FunctionContext&) const;
    void tarjanDiscovery (BasicBlockProperties&, FunctionContext&) const;

    void visit (BasicBlockProperties&, FunctionContext&) const;
    ExitSuggestion visitExit (BasicBlockProperties&, FunctionContext&, uint64_t) const;
    void visitTarjan (BasicBlockProperties&, FunctionContext&) const;
    void visitChild (
        const llvm::BranchInst*
      , unsigned i
      , BasicBlockProperties&
      , FunctionContext&
    ) const;
    void visitChildTarjan (
        BasicBlockProperties&
      , BasicBlockProperties&
      , FunctionContext&
    ) const;
};

#endif
