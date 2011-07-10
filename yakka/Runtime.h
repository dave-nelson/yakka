#ifndef YAKKA_RUNTIME_H_
#define YAKKA_RUNTIME_H_

/**
 * @defgroup Runtime  Yakka runtime: pool management.
 * 
 * The Yakka runtime manages the allocation and caching of pools.
 * 
 * @{
 */
#include <apr_pools.h>
#include <stdbool.h>
#include "Interface.h"

/**
 * Private struct for the Yakka runtime.
 */
typedef struct y_Runtime y_Runtime;
struct y_ObjectClass;
struct y_Error;

/**
 * Create a Runtime, or aquire an existing one.
 *
 * The provided global pool will be checked to see if it has an existing 
 * Runtime, and if so that will be returned. Otherwise a new Runtime will be 
 * created using memory allocated from the global pool.  The Runtime will then 
 * be attached to the global pool for future use.  If the global pool is NULL, 
 * a new one will be created (and destroyed when the Runtime itself is 
 * destroyed).
 *
 * The provided object pool may be used to allocate memory for objects.  If it 
 * is NULL, it will be created as a subpool of the global pool (and destroyed 
 * when the Runtime itself is destroyed).
 *
 * Creating/acquiring a runtime is not thread-safe: when doing so it is 
 * necessary to ensure that the application is not threaded, or that it 
 * creates/acquires its runtime before launching threads, or that some other 
 * synchronisation method is used such as global locking.
 *
 * @param  global_pool  Pool to use as the global pool.  If NULL, a new pool 
 * will be created.
 * @param  objects_pool  Pool from which all per-object pools will be created.
 * If NULL, a new pool will be created from the global pool.
 * @param  pool_buffer_size  The number of pools to keep buffered for reuse.
 * @param  threadsafe  Whether thread safety is to be enforced (using mutexes 
 * for unsafe operations).
 * @return  The Runtime.
 */
y_Runtime * y_Runtime_new (apr_pool_t * global_pool,
        apr_pool_t * objects_pool, int pool_buffer_size, bool threadsafe);

/**
 * Initialise a new type in the Yakka type system.
 *
 * This method is thread-safe, and can accommodate race conditions.  Before 
 * initialising the type, the Runtime is locked and an attempt is made to get 
 * the type, just in case another thread has already created the type.  If 
 * found, the existing type will be returned.  If not, the type will be created 
 * and stored in the runtime in the locked section of code.
 *
 * This method only handles the general steps of type initialisation.  An 
 * init_type callback is to be provided by the caller.  That callback will 
 * typically perform the additional specific steps required to initialise the 
 * type (e.g. override virtual methods etc.).
 *
 * @param  rt  The Runtime that is to create and store the type.
 * @param  type_name  The name under which the type will be stored (and 
 * retrieved later).
 * @param  type_size  The size, in bytes, to be allocated for the type struct.
 * @param  super_type  The super type of the type being created (or NULL if the 
 * type has no super type, but this would be uncommon).
 * @param  init_type  The callback to be used to complete the class-specific 
 * initialisation of the type.
 * @param  type_location  The location in memory where the type is to be stored.
 * @return  The initialised class type.
 */
void * y_Runtime_init_type (y_Runtime * rt, const char * type_name,
        int type_size, void * super_type,
        void (* init_type) (y_Runtime * rt, void * type, void * super_type),
        void ** type_location);

/**
 * Convenience macro: get or initialise a subtype.
 
 * This macro generates code that first searches for the class identified by 
 * name.  If found, it is returned.  Otherwise the type is initialised and 
 * returned.
 *
 * @param  rt  Pointer to the runtime.
 * @param  type_name  Name of the class, as a cstring.
 * @param  Class  The class' struct.
 * @param  get_super  Method to get the super type.
 * @param  type_init  Class initialisation method.
 * @param  type  The name of the variable to which the class stucture will be 
 * assigned.
 * @return  The type identified by type_name.
 */
#define y_GET_OR_CREATE_SUBTYPE(rt, type_name, Class, get_super, type_init,     \
        type)                                               \
    do {                                                    \
        if ( ! type ) {                                     \
            type = y_Runtime_init_type(                     \
                    rt,                                     \
                    type_name,                              \
                    sizeof (Class),                         \
                    get_super (rt),                         \
                    type_init,                              \
                    (void **)& type                         \
                    );                                      \
        }                                                   \
        return (Class *)type;                               \
    } while (0)

/**
 * Enquire: was thread safety specified when the Yakka runtime was instantiated?
 *
 * @param  rt  The Yakka runtime.
 * @return  True if thread safety was enabled; false otherwise.
 */
bool y_Runtime_is_threadsafe (y_Runtime * rt);

/**
 * Get the Yakka runtime's global pool.
 */
apr_pool_t * y_Runtime_get_global_pool (y_Runtime * rt);

/**
 * Create a pool for an object instance.
 */
apr_pool_t * y_Runtime_create_object_pool (y_Runtime * rt, 
        struct y_Error ** error);

/**
 * Return an object pool to the Yakka runtime.
 */
void y_Runtime_free_object_pool (y_Runtime * rt, apr_pool_t * pool);

/**
 * Get the identifier of an interface, by name.
 */
int y_Runtime_get_interface_id (y_Runtime * rt, const char * name);

/**
 * Create a packed set of interface implementations, using the provided 
 * interface specifications.
 *
 * The provided specs are in the form of a NULL-terminated list of 
 * specification name and implementation pairs.  These are re-arranged into a 
 * sparsely-populated array of implementations, indexed by interface ID.
 */
y_Interfaces * y_Runtime_pack_interfaces (y_Runtime * rt,
        y_InterfaceSpec * specs);

/**
 * Lock the resource manager.
 */
void y_Runtime_lock (y_Runtime * rt);

/**
 * Unlock the resource manager.
 */
void y_Runtime_unlock (y_Runtime * rt);

/**
 * Destroy the Runtime.
 *
 * If the global pool was provided during the creation of this Runtime, it will 
 * not be destroyed.  However if the global pool was created during the 
 * creation of this Runtime, it will be destroyed.  The same applies for the 
 * object pool.
 */
void y_Runtime_destroy (y_Runtime * rt);

/** @} */
#endif
