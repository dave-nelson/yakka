#ifndef DELTA_H_
#define DELTA_H_

#include <yakka/Yakka.h>
#include "Alpha.h"
#include "Gamma.h"

typedef struct DeltaClass DeltaClass;
typedef struct Delta {
    Alpha       alpha;
} Delta;

/**
 * Get the class type for Delta.
 */
DeltaClass * Delta_type (y_Runtime * rt);

/**
 * Create a new instance of Delta.
 */
Delta * Delta_new (y_Runtime * rt, int a, const char * b, Gamma * c, 
        const char * d,
        y_Error ** error);

#define DELTA(self)     \
    y_SAFE_CAST_INSTANCE(self, Delta_type, Delta)

void Delta_set_b (void * self, const char * b);

const char * Delta_get_b (void * self);

void Delta_set_c (void * self, Gamma * c);

Gamma * Delta_get_c (void * self);

void Delta_set_d (void * self, const char * d);

const char * Delta_get_d (void * self);

#endif
