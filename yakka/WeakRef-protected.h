#ifndef YAKKA_WEAK_REF_PROTECTED_H_
#define YAKKA_WEAK_REF_PROTECTED_H_

#include "Object-protected.h"
#include "WeakRef.h"

struct y_WeakRefProtected {
    y_ObjectProtected   object;
    void              * target;
};

struct y_WeakRefClass {
    y_ObjectClass object;
};

/**
 * Create a weak reference for an object.
 *
 * Normally this should only be called from within an object.
 */
y_WeakRef * y_WeakRef_new (y_Runtime * rt, void * instance,
        y_Error ** error);

#endif
