#ifndef H_PATH_GROUPER
#define H_PATH_GROUPER

#include <list>
#include <map>
#include <set>

#include "Fact.h"

class PathGrouper {
  public:
    using FactSet = const std::list<std::unique_ptr<Fact>>;
    // timely ordered list of comm-grouped, unit-grouped facts
    using GroupedFactSets =
      std::list<                    // epochs
        std::list<                  // communicators
          std::map<                 // window ranks -> windows
              uint64_t
            , std::set<Fact*>
          >
        >
      >;

    PathGrouper (FactSet&);

    GroupedFactSets getOrderedWindows () const;
  private:
    FactSet& factSet;
};

#endif
