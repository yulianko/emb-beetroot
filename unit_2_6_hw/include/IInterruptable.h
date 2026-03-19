#ifndef IINTERRUPTABLE_H
#define IINTERRUPTABLE_H

#include <cstddef>

#include "esp_attr.h"

class IInterruptable {
  public:
    virtual void IRAM_ATTR onInterrupt() = 0;
    virtual ~IInterruptable() = default;
};

struct IsrContext {
    IInterruptable** observers;
    size_t size;
};

#endif
