#ifndef YAKKA_WEAK_REF_PROTECTED_H_
#define YAKKA_WEAK_REF_PROTECTED_H_

/**
 * @addtogroup WeakRef
 * @{
 *      @defgroup WeakRefProtected  Protected members of WeakRef
 *      @{
 */

#include "Object-protected.h"
#include "WeakRef.h"

/**
 * The protected instance structure of WeakRef.
 */
typedef struct y_WeakRefProtected {
    /** The super class protected structure. */
    y_ObjectProtected   object;
    /** Pointer to the referent: the instance to which this WeakRef refers. */
    void              * target;
} y_WeakRefProtected;

/**
 * The class structure for WeakRef.
 */
struct y_WeakRefClass {
    /** The super class structure. */
    y_ObjectClass object;
};

/**
 * Create a weak reference for an object.
 *
 * Normally this method would not be called directly.  The Object class provides a method @ref y_weak_ref to create a weak reference to an object.   This is because, conceptually, taking a weak reference to an object is an operation that is performed on the object itself, similarly to @ref y_ref.
 *
 * @param  rt  The Yakka runtime.
 * @param  instance  An object instance.
 * @param  error  An error location.  If not NULL, any errors that occur will be assigned to it.
 * @return  The weak reference for the object instance.
 */
y_WeakRef * y_WeakRef_new (y_Runtime * rt, void * instance,
        y_Error ** error);

/**
 * @}
 * @}
 */
#endif
