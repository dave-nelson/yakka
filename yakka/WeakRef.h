#ifndef YAKKA_WEAK_REF_H_
#define YAKKA_WEAK_REF_H_

#include "Object.h"

typedef struct y_WeakRefClass y_WeakRefClass;
typedef struct y_WeakRefProtected y_WeakRefProtected;

typedef struct y_WeakRef {
    y_Object object;
} y_WeakRef;

/**
 * Get the type for WeakReference.
 */
y_WeakRefClass * y_WeakRef_type (y_Runtime * rt);

/**
 * Dereference a weak reference (if possible).
 */
void * y_WeakRef_deref (void * self);

/**
 * Check whether the referent still exists.
 */
bool y_WeakRef_is_set (void * self);

#define y_WEAK_REF(self) \
    y_SAFE_CAST_INSTANCE(self, y_WeakRef_type, y_WeakRef)

#endif
