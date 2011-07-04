#ifndef GAMMA_H_
#define GAMMA_H_

#include <yakka/Yakka.h>

static const int Gamma_default = 42;

typedef struct GammaClass GammaClass;
typedef struct GammaProtected GammaProtected;

typedef struct Gamma {
    y_Object object;
} Gamma;

/**
 * Get the class type for Gamma.
 */
GammaClass * Gamma_type (y_Runtime * rt);

/**
 * Create a new instance of Gamma.
 */
Gamma * Gamma_new (y_Runtime * rt, y_Error ** error);

#define GAMMA(self) \
    y_SAFE_CAST_INSTANCE(self, Gamma_type, Gamma)

#endif
