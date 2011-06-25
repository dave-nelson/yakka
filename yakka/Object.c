#include <stdio.h>
#include <assert.h>
#include "Object-protected.h"
#include "Runtime.h"
#include "WeakRef-protected.h"

static const char * Object_type_name = "Object";
static void * type = NULL;

void y_Object_clear (void * self, bool unref_objects);
void y_clear_object (void * self, bool unref_objects);
apr_status_t y_cleanup_of_last_resort (void * data);
void y_Object_init_type (y_Runtime * rt, void * type, void * super_type);


y_Object *
y_Object_new (y_Runtime * rt,
        y_Error ** error)
{
    return y_create (rt, y_Object_type (rt), error);
}

bool
y_is_a (const void * instance, const void * expected_type)
{
    y_ObjectClass * actual_type = NULL;
    bool match = false;

    if ( !(instance && expected_type && 
                (actual_type = (((y_Object *)instance)->type))) ) {
        return false;
    }

    while ( actual_type && !match) {
        match = (actual_type == expected_type);
        actual_type = actual_type->super;
    }
    return match;
}

void
y_Object_clear (void * self, bool unref_objects)
{
    y_Object * obj = y_OBJECT (self);
    if ( ! obj )
        return;

    if ( unref_objects ) {
        if ( obj->protect->weak_ref ) {
            y_unref (obj->protect->weak_ref);
            obj->protect->weak_ref = NULL;
        }
    }
}

apr_status_t
y_cleanup_of_last_resort (void * data)
{
    y_Object * obj = y_OBJECT (data);
    if ( obj && !obj->protect->deleted ) {
        y_clear_object (obj, false);  /* false: too late for full cleanup */
        obj->protect->deleted = true;
    }
    return APR_SUCCESS;
}

void *
y_create (y_Runtime *rt, const void * class_type,
        y_Error ** error)
{
    y_Object * obj = NULL;
    apr_pool_t * pool = NULL;

    pool = y_Runtime_create_object_pool (rt, error);
    if ( error && *error )
        goto cleanup;
    
    obj = apr_pcalloc (pool, ((y_ObjectClass *)class_type)->instance_size);
    obj->type = (void *)class_type;
    obj->protect = apr_pcalloc (pool, ((y_ObjectClass *)class_type)->protected_size);
    obj->protect->rt = rt;
    obj->protect->pool = pool;

#if APR_HAS_THREADS
    if ( y_Runtime_is_threadsafe (rt) ) {
        if ( y_Error_throw_apr (rt, error, __FILE__, __LINE__,
                    apr_thread_mutex_create (&(obj->protect->mutex),
                        APR_THREAD_MUTEX_DEFAULT, pool)) )
            goto cleanup;
        apr_pool_cleanup_register (pool, obj->protect->mutex,
                (apr_status_t (*)(void *))apr_thread_mutex_destroy, NULL);
    }    
#endif /* APR_HAS_THREADS */
    obj->protect->refcount = 1;
    obj->protect->weak_ref = NULL;

    y_InitMethodList * init = ((y_ObjectClass *)class_type)->init;

    if ( init ) {
        int i;
        for ( i = 0; i < init->size; i++ ) {
            struct y_InitMethod method = init->methods[i];

            if ( y_bless (obj, method.type ) ) {
                method.exec (obj, rt, pool, error);
                if ( error && *error )
                    goto cleanup;
            }
        }
    }
    obj->type = (void *)class_type;
    apr_pool_cleanup_register (pool, obj, y_cleanup_of_last_resort, NULL);

    return obj;

/* Failure: just delete underlying pool and return NULL */
cleanup:
    if ( pool )
        y_Runtime_free_object_pool (obj->protect->rt, obj->protect->pool);
    return NULL;
}

void
y_clear (void * self)
{
    y_clear_object (self, true);
}

void
y_clear_object (void * self, bool unref_objects)
{
    y_Object * obj = y_OBJECT (self);
    if ( ! obj )
        return;

    y_ObjectClass * type = obj->type;
    y_ClearMethodList * clear = type->clear;

    if ( clear ) {
        int i;
        for ( i = clear->size; i > 0; i-- ) {
            struct y_ClearMethod method = clear->methods[i - 1];
    
            if ( y_bless (obj, method.type) ) {
                method.exec (obj, unref_objects);
            }
        }
        y_bless (obj, type);
    }
}

void
y_assign (void *to, const void *from,
        y_Error ** error)
{
    if ( (! to) || (! from) )
        return;

    y_ObjectClass * type = ((y_Object *)to)->type;
    y_AssignMethodList * assign = type->assign;

    if ( assign && ((y_Object *)to)->type == ((y_Object *)from)->type ) {
        y_clear_object (to, true);
        int i;
        for ( i = 0; i < assign->size; i++ ) {
            struct y_AssignMethod method = assign->methods[i];

            if ( y_bless (to, method.type) && y_bless (from, method.type)) {
                method.exec (to, from, error);
                if ( error && *error )
                    break;
            }
        }
        y_bless (to, type);
        y_bless (from, type);
    }
}

void *
y_copy (const void * self,
        y_Error ** error)
{
    const y_Object * obj = (const y_Object *)self;
    if ( ! obj )
        return NULL;
    void * to = y_create (obj->protect->rt, obj->type, error);
    if ( error && *error )
        goto cleanup;
    y_assign(to, obj, error);
    if ( error && *error )
        goto cleanup;

    return to;

cleanup:
    if ( to )
        y_unref (to);
    return NULL;
}

void
y_lock (void * self)
{
#if APR_HAS_THREADS
    y_Object * obj = y_OBJECT (self);
    
    if ( obj && obj->protect->mutex ) {
        apr_thread_mutex_lock (obj->protect->mutex);
    }
#endif /* APR_HAS_THREADS */
}

bool
y_try_lock (void * self)
{
    bool acquired = false;
#if APR_HAS_THREADS
    y_Object * obj = y_OBJECT (self);
        if ( obj ) {
        apr_status_t status = apr_thread_mutex_trylock (obj->protect->mutex);
        if ( ! APR_STATUS_IS_EBUSY (status)) {
            acquired = true;
        }
    }
#endif /* APR_HAS_THREADS */
    return acquired;
}

void
y_unlock (void * self)
{
#if APR_HAS_THREADS
    y_Object * obj = y_OBJECT (self);

    if ( obj && obj->protect->mutex ) {
        apr_thread_mutex_unlock (obj->protect->mutex);
    }
#endif /* APR_HAS_THREADS */
}

void *
y_ref (void * self)
{
    y_Object * obj = y_OBJECT (self);
    if ( ! obj )
        return NULL;
    void * ref = NULL;
    bool has_weak_ref = obj->protect->weak_ref;

    if ( obj->protect->refcount > 0 ) {
        if ( has_weak_ref ) {
            y_lock (obj->protect->weak_ref);
        }
        y_lock (obj);
        if ( obj->protect->refcount > 0 ) {  /* check again in case changed */
            obj->protect->refcount += 1;
            ref = obj;
        }
        y_unlock (obj);
        if ( has_weak_ref ) {
            y_unlock (obj->protect->weak_ref);
        }
    }
    return ref;
}

void
y_unref (void * self)
{
    y_Object * obj = y_OBJECT (self);
    if ( ! obj )
        return;
    bool has_weak_ref = obj->protect->weak_ref;
    bool do_cleanup = false;


    if ( obj->protect->refcount > 0 ) {
        if ( has_weak_ref ) {
            y_lock (obj->protect->weak_ref);
        }
        y_lock (obj);
        if ( obj->protect->refcount > 0 ) {  /* check again in case changed */
            obj->protect->refcount -= 1;
            if ( obj->protect->refcount == 0 ) {
                do_cleanup = true;
            }
        }
        y_unlock (obj);
        if ( has_weak_ref ) {
            y_unlock (obj->protect->weak_ref);
        }
    }
    if ( do_cleanup ) {
        y_destroy (obj);
    }
}

void *
y_weak_ref (void * self)
{
    y_Object * obj = y_OBJECT (self);
    if ( ! obj )
        return NULL;
    y_Error * error = NULL;

    if ( ! obj->protect->weak_ref ) {
        y_lock (obj);
        if ( ! obj->protect->weak_ref ) {
            obj->protect->weak_ref = y_WeakRef_new (obj->protect->rt, obj, &error);
            if ( error )
                y_unref (error);
        }
        y_unlock (obj);
    }
    return y_ref (obj->protect->weak_ref);
}

y_Runtime *
y_get_runtime (const void * self)
{
    if ( self ) {
        y_Object * obj = (y_Object *)self;
        return obj->protect->rt;
    }
    else {
        return NULL;
    }
}

void
y_destroy (void * self)
{
    y_Object * obj = y_OBJECT (self);
    if ( obj && !obj->protect->deleted ) {
        y_clear_object (obj, true);
        obj->protect->deleted = true;
        y_Runtime_free_object_pool (obj->protect->rt, obj->protect->pool);
    }
}

void
y_init_type (y_Runtime * rt, void * type, void * super_type, const char * name, 
        size_t class_size, size_t instance_size, size_t protected_size,
        void (* init_method) (void * self, y_Runtime * rt, apr_pool_t * pool,
            y_Error ** error),
        void (* assign_method) (void * to, const void * from, y_Error ** error),
        void (* clear_method) (void * self, bool unref_objects))
{
    y_ObjectClass * object_type = (y_ObjectClass *)type;
    y_ObjectClass * super_type_ = (y_ObjectClass *)super_type;

    object_type->super = super_type;
    object_type->name = name;
    object_type->class_size = class_size;
    object_type->instance_size = instance_size;
    object_type->protected_size = protected_size;

    if ( init_method ) {
        object_type->init = (y_InitMethodList *)y_MethodList_extend (
                rt,
                (y_MethodList *)(super_type_ ? super_type_->init : NULL),
                object_type,
                init_method
                );
    }
    if ( assign_method ) {
        object_type->assign = (y_AssignMethodList *)y_MethodList_extend (
                rt,
                (y_MethodList *)(super_type_ ? super_type_->assign : NULL),
                object_type,
                assign_method
                );
    }
    if ( clear_method ) {
        object_type->clear = (y_ClearMethodList *)y_MethodList_extend (
                rt,
                (y_MethodList *)(super_type_ ? super_type_->clear : NULL),
                object_type,
                clear_method
                );
    }
}

void
y_Object_init_type (y_Runtime * rt, void * type, void * super_type)
{
    y_init_type (
            rt,
            type,
            super_type,
            Object_type_name,
            sizeof (y_ObjectClass),
            sizeof (y_Object),
            sizeof (y_ObjectProtected),
            NULL,
            NULL,
            y_Object_clear
            );
}

y_ObjectClass *
y_Object_type (y_Runtime *rt)
{
    if ( ! type ) {
        type = y_Runtime_init_type (
                rt,                     /* Runtime */
                Object_type_name,       /* Name of type */
                sizeof (y_ObjectClass), /* Size of class struct */
                NULL,                   /* Superclass */
                y_Object_init_type,     /* Type initialisation callback */
                &type
                );
    }
    return type;
}

