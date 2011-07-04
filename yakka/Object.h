#ifndef YAKKA_OBJECT_H_
#define YAKKA_OBJECT_H_

/** @defgroup Object Object
 *
 * The base class of the Yakka type system.
 *
 * @{
 */

#include <stdlib.h>
#include <stdbool.h>
#include <apr_pools.h>

typedef struct y_ObjectClass y_ObjectClass;
typedef struct y_ObjectProtected y_ObjectProtected;

struct y_Error;
struct y_Runtime;

/**
 * Structure for an object instance.
 */
struct y_Object {
    /** The class type of an object (polymorphic) */
    void              * type;
    y_ObjectProtected * protect;
};
typedef struct y_Object y_Object;

/**
 * Get the class type for Object.
 */
y_ObjectClass * y_Object_type (struct y_Runtime * rt);

/**
 * Get an instance of an Object.
 * 
 * This is permissible, but in practice an Object instance doesn't do anything.
 */
y_Object * y_Object_new (struct y_Runtime * rt, struct y_Error ** error);

/**
 * Tests whether the given object is of type or any super type
 */
bool y_is_a (const void * self, const void * type);

/**
 * Clear the contents of given object: unset or free all attributes.
 *
 * @param  self  The object to clear.
 */
void y_clear (void * self);

/**
 * Create a copy of the given object.
 *
 * A new object of the same type as the provided instance will be instantiated, 
 * and all values will be copied to it.  It will then be returned.
 *
 * @param  self  The instance to which attributes should be copied.
 * @param  error  An error location, to be populated with any errors that may 
 * occur.  If NULL no errors will be reported.
 * @return  A pointer to the newly created instance, or NULL if copying is 
 * forbidden.
 */
void * y_copy (const void * self, struct y_Error ** error);

/**
 * Assign from one object to another of the same type.
 *
 * Assignment is only applicable where two instance objects are of the same 
 * type.
 *
 * @param  to  The instance to which attributes should be copied.
 * @param  from  The instance from which attributes should be copied.
 * @param  error  An error location, to be populated with any errors that may 
 * occur.  If NULL no errors will be reported.
 * @return  A pointer to the "to" instance, or NULL if assignment is forbidden.
 */
void * y_assign (void * to, const void * from, struct y_Error ** error);

/**
 * Destroy an object: clear it then free its allocated memory.
 */
void y_destroy (void * self);

/**
 * Lock an object.
 */
void y_lock (void * self);

/**
 * Attempt to gain a lock on an object.
 * 
 * @param  self  The object to attempt to lock.
 * @return   True on success, false if the object cannot be locked.
 */
bool y_try_lock (void * self);

/**
 * Unlock an object.
 */
void y_unlock (void * self);

/**
 * Acquire a reference to an object, increasing its reference count.
 */
void * y_ref (void * self);

/**
 * Relinquish an object reference
 */
void y_unref (void * self);

/**
 * Acquire a weak reference to an object.
 */
void * y_weak_ref (void * self);

/**
 * Get the runtime from an object.
 */
struct y_Runtime * y_get_runtime (const void * self);

/**
 * Get the implementation that an instance's class provides for an interface, 
 * by interface ID.
 *
 * This method will return NULL if the instance's class doesn't provide an 
 * implementation for the specified interface.
 *
 * @param  self  An object instance.
 * @param  name  Name of an interface.
 * @return  A pointer to the instance's implementation of the specified 
 * interface, or NULL if not implemented.
 */
void * y_get_implementation (const void * self, int interface_id);

/**
 * Get the implementation that an instance's class provides for an interface, 
 * by name.
 *
 * This method will return NULL if the instance's class doesn't provide an 
 * implementation for the specified interface.  Note that this method is 
 * slightly less efficient than y_get_implementation because it is necessary to 
 * look up the interface ID for the corresponding name.
 *
 * @param  self  An object instance.
 * @param  name  Name of an interface.
 * @return  A pointer to the instance's implementation of the specified 
 * interface, or NULL if not implemented.
 */
void * y_get_implementation_by_name (const void * self, const char * name);

#define y_SAFE_CAST_INSTANCE(self, get_type, cast_as)               \
    ( (self && y_is_a (self, get_type (y_get_runtime (self)))) ?    \
      (cast_as *)self : NULL )

/* Cast an instance as an Object */
#define y_OBJECT(self) \
    y_SAFE_CAST_INSTANCE(self, y_Object_type, y_Object)

/** @} */
#endif
