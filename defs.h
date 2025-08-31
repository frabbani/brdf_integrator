#pragma once

#include <stdint.h>

#define CLAMP(a, min, max) \
  {                        \
    if ((a) < (min))       \
      (a) = (min);         \
    else if ((a) > (max))  \
      (a) = (max);         \
  }
