#ifndef GAMMA_PROTECTED_H_
#define GAMMA_PROTECTED_H_

#include "Gamma.h"
#include <yakka/Object-protected.h>

struct GammaProtected {
    y_ObjectProtected  object;
};
#define GAMMA_PROTECTED(self)   \
    ((GammaProtected *)Y_OBJECT_PROTECTED (self))

struct GammaClass {
    y_ObjectClass       object;
};

#endif
