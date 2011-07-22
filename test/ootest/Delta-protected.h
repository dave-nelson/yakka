#ifndef DELTA_PROTECTED_H_
#define DELTA_PROTECTED_H_

#include "Delta.h"
#include "Gamma.h"
#include "Alpha-protected.h"

struct DeltaPrivate;

typedef struct DeltaProtected {
    AlphaProtected      alpha;

    /* Some protected variables */
    char    * b;
    Gamma   * c;

    /* Private data */
    struct DeltaPrivate * priv;
} DeltaProtected;

#define DELTA_PROTECTED(self)   \
    ((DeltaProtected *)y_OBJECT_PROTECTED (self))

struct DeltaClass {
    AlphaClass      alpha;
};

void Delta_set (Delta * self, int a, const char * b, Gamma * c,
        const char * d,
        y_Error ** error);

#endif
