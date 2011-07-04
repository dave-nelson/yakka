#include <assert.h>
#include <apr_thread_mutex.h>
#include <apr_hash.h>
#include "Runtime.h"
#include "Object-protected.h"
#define APR_WANT_MEMFUNC
#include <apr_want.h>

typedef struct y_Runtime {
    apr_pool_t         * global_pool;
    bool                 cleanup_global; /* need to clean up */
    apr_pool_t         * objects_pool;
    bool                 cleanup_object; /* need to clean up */
    bool                 threadsafe;
    apr_thread_mutex_t * mutex;
    /* Pool trash stack */
    apr_pool_t **        pool_buffer;
    int                  pool_buffer_size;
    int                  pool_buffer_pos;
    /* Interface identifiers */
    apr_hash_t         * interface_ids;
    int                  interface_size;
} y_RuntimePrivate;

/**
 * For internal use only (i.e. while the runtime is already locked): get the 
 * interface ID for the provided name.
 */
int y_Runtime_get_interface_id_nolock (y_Runtime * rt, const char * name);

y_Runtime *
y_Runtime_new (apr_pool_t * global_pool, apr_pool_t * objects_pool, 
        int pool_buffer_size, bool threadsafe)
{
    apr_pool_t * gpool = global_pool;
    bool gcleanup = false;
    apr_pool_t * opool = objects_pool;
    bool ocleanup = false;
    y_Runtime * rt = NULL;

    /* Create pools if required */
    if ( ! gpool ) {
        apr_pool_create (&gpool, NULL);
        gcleanup = true;
    }
    if ( ! opool ) {
        apr_pool_create (&opool, gpool);
        ocleanup = true;
    }

    rt = apr_pcalloc (gpool, sizeof (y_Runtime));
    
    rt->global_pool = gpool;
    rt->cleanup_global = gcleanup;
    rt->objects_pool = opool;
    rt->cleanup_object = ocleanup;

    rt->threadsafe = threadsafe;

    rt->pool_buffer_size = pool_buffer_size;
    if ( pool_buffer_size > 0 ) {
        rt->pool_buffer = apr_pcalloc (gpool, 
                pool_buffer_size * sizeof (apr_pool_t *));
        rt->pool_buffer_pos = 0;
    }
#if APR_HAS_THREADS
    if ( rt->threadsafe ) {
        apr_thread_mutex_create (&(rt->mutex),
                APR_THREAD_MUTEX_DEFAULT, gpool);
    }
#endif /* APR_HAS_THREADS */

    rt->interface_ids = apr_hash_make (gpool);
    rt->interface_size = 0;

    return rt;
}

void
y_Runtime_lock (y_Runtime * rt)
{
#if APR_HAS_THREADS
    if ( rt->mutex ) {
        apr_thread_mutex_lock (rt->mutex);
    }
#endif /* APR_HAS_THREADS */
}

void
y_Runtime_unlock (y_Runtime * rt)
{
#if APR_HAS_THREADS
    if ( rt->mutex ) {
        apr_thread_mutex_unlock (rt->mutex);
    }
#endif /* APR_HAS_THREADS */
}

apr_status_t
y_erase_type (void * data)
{
    void ** type_location = (void **)data;
    if ( type_location ) {
        *type_location = NULL;
    }
    return APR_SUCCESS;
}

int
y_Runtime_get_interface_id_nolock (y_Runtime * rt, const char * name)
{
    int * id = NULL;

    id = (int *)apr_hash_get (rt->interface_ids, name, APR_HASH_KEY_STRING);
    if ( ! id ) {
        id = apr_pcalloc (rt->global_pool, sizeof (int));
        *id = ++(rt->interface_size);
        apr_hash_set (rt->interface_ids, name, APR_HASH_KEY_STRING, id);
    }
    return (id ? *id : 0);
}

int
y_Runtime_get_interface_id (y_Runtime * rt, const char * name)
{
    int * id_ptr = NULL;
    int id = 0;

    id_ptr = (int *)apr_hash_get (rt->interface_ids, name, APR_HASH_KEY_STRING);
    if ( ! id_ptr ) {
        y_Runtime_lock (rt);
        id = y_Runtime_get_interface_id_nolock (rt, name);
        y_Runtime_unlock (rt);
    }
    else {
        id = *id_ptr;
    }
    return id;
}

y_Interfaces *
y_Runtime_pack_interfaces (y_Runtime * rt, y_InterfaceSpec * specs)
{
    int i = 0;
    int max_id = 0;
    y_Interfaces * interfaces = NULL;

    while ( specs[i].name ) {
        int spec_id;
        y_InterfaceSpec spec = specs[i];
        spec_id = y_Runtime_get_interface_id_nolock (rt, spec.name);
        if ( spec_id > max_id ) {
            max_id = spec_id;
        }
        i++;
    }
    if ( max_id > 0 ) {
        interfaces = apr_pcalloc (rt->global_pool, sizeof (y_Interfaces));
        interfaces->size = max_id + 1;
        interfaces->vtables = apr_pcalloc (rt->global_pool,
                (interfaces->size) * sizeof (void *));
        i = 0;
        while ( specs[i].name ) {
            y_InterfaceSpec spec = specs[i];
            interfaces->
                vtables[y_Runtime_get_interface_id_nolock (rt, spec.name)] =
                spec.vtable;
            i++;
        }
    }
    return interfaces;
}

void *
y_Runtime_init_type (y_Runtime * rt, const char * type_name,
        int type_size, void * super_type,
        void (* init_type) (y_Runtime * rt, void * type, void * super_type),
        void ** type_location)
{
    y_ObjectClass * type = NULL;

    y_Runtime_lock (rt);
    if ( ! *type_location ) {
        /* Allocate memory for the type and copy the supertype over it */
        type = apr_pcalloc (rt->global_pool, type_size);
        if ( super_type ) {
            int super_size = ((y_ObjectClass *)super_type)->class_size;
            assert (! (super_size > type_size));
            memcpy (type, super_type, super_size);
        }
        /* Perform type-specific initialisation */
        init_type (rt, type, super_type);
        /* Keep this type */
        apr_pool_cleanup_register (rt->global_pool, type_location,
                y_erase_type, apr_pool_cleanup_null);
        *type_location = type;
    }
    y_Runtime_unlock (rt);

    return type;
}

apr_pool_t *
y_Runtime_get_global_pool (y_Runtime * rt)
{
    return rt->global_pool;
}

bool
y_Runtime_is_threadsafe (y_Runtime * rt)
{
    return rt->threadsafe;
}

apr_pool_t *
y_Runtime_create_object_pool (y_Runtime * rt, y_Error ** error)
{
    apr_pool_t * pool = NULL;
    
    if ( rt->pool_buffer_size ) {
        y_Runtime_lock (rt);
        
        pool = rt->pool_buffer[rt->pool_buffer_pos];
        rt->pool_buffer[rt->pool_buffer_pos] = NULL;
        rt->pool_buffer_pos = ( (--(rt->pool_buffer_pos)) < 0 ? 
                rt->pool_buffer_size - 1 : rt->pool_buffer_pos );
        if ( ! pool ) {
            apr_pool_create (&pool, rt->objects_pool);
        }
        
        y_Runtime_unlock (rt);
    }
    else {
        apr_pool_create (&pool, rt->global_pool);
    }

    return pool;
}

void
y_Runtime_free_object_pool (y_Runtime * rt, apr_pool_t * pool)
{
    apr_pool_t * old = NULL;

    apr_pool_clear (pool);

    if ( rt->pool_buffer_size ) {
        y_Runtime_lock (rt);

        rt->pool_buffer_pos =
            ( (++(rt->pool_buffer_pos)) < rt->pool_buffer_size ? 
              rt->pool_buffer_pos : 0 );
        old = rt->pool_buffer[rt->pool_buffer_pos];
        rt->pool_buffer[rt->pool_buffer_pos] = pool;

        y_Runtime_unlock (rt);

        if ( old ) {
            apr_pool_destroy (old);
        }
    }
    else {
        apr_pool_destroy (pool);
    }
    return;
}

void
y_Runtime_destroy (y_Runtime * rt)
{
    if ( rt->cleanup_object ) {
        apr_pool_destroy (rt->objects_pool);
    }
    if ( rt->cleanup_global ) {
        apr_pool_destroy (rt->global_pool);
    }
}
