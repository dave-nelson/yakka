#ifndef ALPHA_PROTECTED_H_
#define ALPHA_PROTECTED_H_

#include "Alpha.h"
#include <yakka/Object-protected.h>

struct AlphaProtected {
    y_ObjectProtected   object;
};

#define ALPHA_PROTECTED(self)   \
    ((AlphaProtected *)y_OBJECT_PROTECTED (self))

struct AlphaClass {
    y_ObjectClass       object;
};

#endif
