#ifndef YAKKA_WEAK_REF_H_
#define YAKKA_WEAK_REF_H_

/** @defgroup WeakRef WeakRef 
 * 
 * A reference to an object that does not prevent an object from being 
 * destroyed.
 *
 * As long as the referent (some other object) exists, its weak reference can 
 * be used to obtain a referenced pointer to it.  However the existence of the 
 * weak reference does not prevent the object from being destroyed when all 
 * references to it are released (a weak reference does not hold a strong 
 * reference to its referent).  If that occurs, attempting to obtain a 
 * referenced pointer to the referent will return NULL.
 *
 * Weak references are useful in dealing with objects that may go away at some 
 * point, or for avoiding cyclical references (i.e. objects that refer to each 
 * other, meaning that neither can be destroyed).
 *
 * The creation of a weak reference for an object instance is handled via the 
 * Object API method @ref y_weak_ref.
 * @{
 */

#include "Object.h"

/**
 * Structure for the WeakRef class.
 */
typedef struct y_WeakRefClass y_WeakRefClass;

/** Structure for a WeakRef instance. */
typedef struct y_WeakRef {
    /** The super class structure for a weak reference instance: 
     * @ref y_Object. */
    y_Object object;
} y_WeakRef;

/**
 * Get the type for WeakReference.
 *
 * @param  rt  The Yakka runtime.
 * @return  The class structure for weak reference.
 */
y_WeakRefClass * y_WeakRef_type (y_Runtime * rt);

/**
 * Dereference a weak reference (if possible).
 *
 * If the referent still exists, this method will return a referenced (strong) 
 * pointer to that object, otherwise NULL will be returned.  The reference 
 * obtained in this manner should eventually be unreferenced (@ref y_unref) 
 * when no longer needed, just as with any other strong reference.
 * 
 * @param  self  A weak reference.
 * @return  A referenced (strong) pointer to the referent, or NULL if it no 
 * longer exists.
 */
void * y_WeakRef_deref (void * self);

/**
 * Check whether the referent still exists.
 *
 * @param  self  A weak reference.
 * @return  True if the referent still exists, false otherwise.
 */
bool y_WeakRef_is_set (void * self);

/**
 * Safely cast an object to a weak reference.
 */
#define y_WEAK_REF(self) \
    y_SAFE_CAST_INSTANCE(self, y_WeakRef_type, y_WeakRef)

/**
 * @}
 */
#endif
