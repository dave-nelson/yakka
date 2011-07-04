#ifndef ALPHA_H_
#define ALPHA_H_

#include <yakka/Yakka.h>

typedef struct AlphaClass AlphaClass;
typedef struct AlphaProtected AlphaProtected;

typedef struct Alpha {
    y_Object object;
    int      iota;
} Alpha;

/**
 * Get the class type for Alpha.
 */
AlphaClass * Alpha_type (y_Runtime * rt);

/**
 * Create a new instance of Alpha.
 */
Alpha * Alpha_new (y_Runtime * rt, int iota, y_Error ** error);

#define ALPHA(self) \
    y_SAFE_CAST_INSTANCE(self, Alpha_type, Alpha)

#endif
