#include "PathGrouper.h"

PathGrouper::PathGrouper (PathGrouper::FactSet &FactSet)
  : factSet(FactSet)
{}

PathGrouper::GroupedFactSets PathGrouper::getOrderedWindows () const {
  std::list<std::set<Fact*>> orderedFacts;

  for (auto &fact : factSet) {
    if (!fact->isType(Fact::Type::Win)) {
      continue;
    }

    auto SM = fact->getPath();

    if (nullptr != SM->getViolatingInstruction() || !SM->hasFinalState()) {
      continue;
    }

    if (0 == orderedFacts.size()) {
      orderedFacts.push_back({fact.get()});
      continue;
    }

    auto it = orderedFacts.begin();
    bool newListElement = false;

    for (; it != orderedFacts.end(); ++it) {
      auto someElement = *(it->begin());

      if (fact->isAncestor(someElement)) {
        continue;
      } else if (someElement->isAncestor(fact.get())) {
        newListElement = true;
        break;
      } else {
        it->insert(fact.get());
        break;
      }
    }

    if (newListElement) {
      if (orderedFacts.begin() == it) {
        orderedFacts.push_front({});
        orderedFacts.front().insert(fact.get());
      } else {
        auto newIt = orderedFacts.insert(it, {});
        newIt->insert(fact.get());
      }
    }
  }

  GroupedFactSets groupedFacts;

  for (auto &factEpoch : orderedFacts) {
    std::list<std::set<Fact*>> commGroups;

    for (auto fact : factEpoch) {
      if (0 == commGroups.size()) {
        commGroups.push_back({fact});
      } else {
        bool newCommGroup = true;

        for (auto &commGroup : commGroups) {
          auto someElement = *(commGroup.begin());

          if (someElement->knowsCommunicator(fact->getCommunicator())) {
            commGroup.insert(fact);
            newCommGroup = false;
          }
        }

        if (newCommGroup) {
          commGroups.push_back({fact});
        }
      }
    }

    std::list<std::map<uint64_t, std::set<Fact*>>> groupedWindowCommunicators;

    for (auto &commGroup : commGroups) {
      std::map<uint64_t, std::set<Fact*>> rankMap;

      for (auto fact : commGroup) {
        uint64_t rank = fact->getUnitCount();
        auto searchResult = rankMap.find(rank);

        if (rankMap.end() != searchResult) {
          searchResult->second.insert(fact);
        } else {
          rankMap[rank] = {fact};
        }
      }

      groupedWindowCommunicators.emplace_back(std::move(rankMap));
    }

    groupedFacts.emplace_back(std::move(groupedWindowCommunicators));
  }

  return groupedFacts;
}
