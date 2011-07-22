#ifndef YAKKA_OBJECT_PROTECTED_H_
#define YAKKA_OBJECT_PROTECTED_H_

/**
 * @addtogroup Object
 * @{
 *      @defgroup ObjectProtected  Protected members of Object
 *      @{
 */

#include "Object.h"
#include "MethodList.h"
#include "Error.h"
#include "Interface.h"
#include <apr_thread_mutex.h>
#include <assert.h>

struct y_ObjectClass;
struct y_WeakRef;

/**
 * Object: the top-level instance in the Yakka type system.
 *
 * An Object instance only contains the information required to manage the 
 * instance, such as its pool, mutex, reference count etc.
 */
typedef struct y_ObjectProtected {
    /** Pointer to the Yakka runtime. */
    struct y_Runtime         * rt;
    /** The pool for this instance. */
    apr_pool_t               * pool;
    /** The mutex for this instance (NULL if threads are not enabled). */
    apr_thread_mutex_t       * mutex;
    /** The number of references to this instance being held elsewhere. */
    int                        refcount;
    /** The weak reference to this instance, if it exists; otherwise NULL. */
    struct y_WeakRef         * weak_ref;
    /** Whether this object is in the process of being deleted. */
    bool                       deleted;
} y_ObjectProtected;

/**
 * Convenience macro to get the protected struct of an Object instance.
 *
 * @param  self  An instance of Object.
 * @return  The protected data structure of the instance, if the instance is 
 * not NULL; otherwise NULL.
 */
#define y_OBJECT_PROTECTED(self) \
    ( self ? ((y_ObjectProtected *)((y_Object *)self)->protect) : NULL )

/**
 * Initialisation method chain, used to perform default initialisation (no-arg 
 * constructor) of a new instance.
 *
 * This method chain is used by y_create() to initialise a new object.  
 * Instance variables may be allocated and/or set to default values.  This 
 * includes any protected structures.
 *
 * The chain is executed from deep to shallow, for any types that define an 
 * init method.
 */
typedef struct y_InitMethodList {
    /** The number of methods in this method list. */
    size_t    size;
    /** The initialisation methods in this method list. */
    struct y_InitMethod {
        /** The type that defined this method. */
        void    * type;
        /** Pointer to the implementation of this method. */
        void   (* exec) (void * self, y_Error ** error);
    } * methods;
} y_InitMethodList;

/**
 * Assign method chain, used to copy the contents of one object to another.
 *
 * This method chain is used by y_assign() to copy the members of one object to 
 * another.  (Note that y_assign() will call y_clear() first to clear any 
 * existing values).  The chain is executed from deep to shallow, for any types 
 * that define an init method.
 *
 * Note that while ObjectClass provides the facility for an assign method, it 
 * does not need to provide one itself because Objects do not contain any 
 * instance variables that need to be copied from one Object to another.
 */
typedef struct y_AssignMethodList {
    /** The number of methods in this method list. */
    size_t    size;
    /** The initialisation methods in this method list. */
    struct y_AssignMethod {
        /** The type that defined this method. */
        void    * type;
        /** Pointer to the implementation of this method. */
        void * (* exec) (void * to, const void * from,
                y_Error ** error);
    } * methods;
} y_AssignMethodList;

/**
 * Clear method chain, used to clear the instance variables of an Object.
 *
 * This method chain is used by y_clear() to clear any existing member 
 * variables before assigning or deleting an instance. The chain is executed 
 * from shallow to deep, for any types that define a clear method.
 *
 * Note that while ObjectClass provides the facility for a clear method, it 
 * does not need to provide one itself because Objects do not contain any 
 * instance variables that need to be cleared.
 */
typedef struct y_ClearMethodList {
    /** The number of methods in this method list. */
    size_t    size;
    /** The initialisation methods in this method list. */
    struct y_ClearMethod {
        /** The type that defined this method. */
        void    * type;
        /** Pointer to the implementation of this method. */
        void   (* exec) (void * self, bool unref_objects);
    } * methods;
} y_ClearMethodList;

/**
 * The class structure for Object.
 */
typedef struct y_ObjectClass {
    /** The super class structure for this class. */
    void               * super;
    /** The human-readable name of the class. */
    const char         * name;
    /** Pointer to the Yakka runtime. */
    y_Runtime          * rt;
    /** The size of the class struct (bytes). */
    size_t               class_size;
    /** The size of the public instance struct. */
    size_t               instance_size;  /* Size of an instance */
    /** The size of the protected instance struct. */
    size_t               protected_size;   /* Size of the protected data structure */

    /** List of initialisation methods for this class. */
    y_InitMethodList   * init;
    /** List of assignment methods for this class. */
    y_AssignMethodList * assign;
    /** List of clear methods for this class. */
    y_ClearMethodList  * clear;
    /** The interfaces that this class implements (NULL if none). */
    y_Interfaces       * interfaces;
} y_ObjectClass;

/**
 * Convenience method to initialise a class structure.
 *
 * Defining a class structure for a type involves setting various fields with 
 * information about the type.  This method takes the required information as 
 * parameters and sets the fields appropriately for Yakka's Object class and 
 * any types derived from Object.  (Note that subclasses may have further class 
 * parameters to set, such as virtual methods, so further initialisation may be 
 * required beyond what this method provides.)
 *
 * @param  rt  The Yakka Runtime.
 * @param  type  The type to be initialised.
 * @param  super_type  The super type of the type to be initialised.
 * @param  name  The human-readable name of the type.
 * @param  class_size  The size (in bytes) of the type's class struct.
 * @param  instance_size  The size of the type's public instance struct.
 * @param  protected_size  The size of the type's protected instance struct.
 * @param  init_method   An initialisation method, to be added to the chain of 
 * initialisation methods for the default, no-arg constructor.  (NULL if no 
 * specific initialisation is required.)
 * @param  assign_method  An assign method, to be added to the chain of 
 * assignment methods, to copy values from one instance to another. (NULL if no 
 * specific assignment is required.)
 * @param  clear_method  A clear method, to be added to the chain of clear 
 * methods (NULL if no specific clear is required.)
 */
void y_init_type (y_Runtime * rt, void * type, void * super_type, const char * name, 
        size_t class_size, size_t instance_size, size_t protected_size,
        void   (* init_method  ) (void * self, y_Error ** error),
        void * (* assign_method) (void * to, const void * from, y_Error ** error),
        void   (* clear_method ) (void * self, bool unref_objects));

/**
 * No-arg constructor for a given type.
 */
void * y_create (struct y_Runtime * rt, const void * class_type,
        struct y_Error ** error);

/**
 * Get the type of an instance, cast as a particular class. 
 *
 * @param  Class  The name of a class type.
 * @param  instance  An object instance.
 */
#define TYPE_AS_CLASS(Class, instance) \
    ((Class *)((y_Object *)instance)->type)

/**
 * The type of an instance, cast as y_ObjectClass
 */
#define TYPE_AS_OBJECT(instance) \
    (TYPE_AS_CLASS (y_ObjectClass, instance))

/**
 * Change the type of an object to another type.
 *
 * @param  instance  An object instance.
 * @param  retype  The type to which the instance should be re-cast.
 */
#define y_bless(instance, retype)    \
    (retype ? ((((y_Object *)instance)->type = retype)) : NULL)

/** 
 * @}
 * @}
 */
#endif
