#ifndef H_WINDOW_MODE_CHECK
#define H_WINDOW_MODE_CHECK

#include "Fact.h"

class WindowModeCheck {
  public:
    WindowModeCheck (const std::set<Fact*>&);
    bool validate () const;

  private:
    const std::set<Fact*> &windows;
};

#endif
