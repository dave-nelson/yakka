#include "WeakRef-protected.h"
#include "Object-protected.h"

static char * weak_ref_type_name = "y_WeakRef";
static y_WeakRefClass * weak_ref_class = NULL;

y_WeakRef * y_WeakRef_new (y_Runtime * rt, void * instance,
        y_Error ** error)
{
    y_WeakRef * ref = y_create (rt, y_WeakRef_type (rt), error);
    y_WeakRefProtected * prot;

    if ( ! error || ! *error ) {
        prot = (y_WeakRefProtected *)y_OBJECT_PROTECTED (ref);
        prot->target = instance;
    }
    return ref;
}

void *
y_WeakRef_deref (void * self)
{
    y_WeakRef * ref = y_WEAK_REF (self);
    y_WeakRefProtected * prot = (y_WeakRefProtected *)y_OBJECT_PROTECTED (ref);
    void * instance = NULL;

    if ( prot->target ) {
        y_lock (self);
        if ( prot->target )
            instance = prot->target;
        y_unlock (self);
    }
    if ( instance )
        return y_ref (instance);
    else
        return NULL;
}

bool
y_WeakRef_is_set (void * self)
{
    y_WeakRef * ref = y_WEAK_REF (self);
    y_WeakRefProtected * prot = (y_WeakRefProtected *)y_OBJECT_PROTECTED (ref);
    return ( prot->target ? true : false );
}

void
y_WeakRef_init_type (y_Runtime * rt, void * type, void * super_type)
{
    y_init_type (
            rt,
            type,
            super_type,
            weak_ref_type_name,
            sizeof (y_WeakRefClass),
            sizeof (y_WeakRef),
            sizeof (y_WeakRefProtected),
            NULL,  /* init */
            NULL,  /* assign */
            NULL   /* clear */
            );
}

y_WeakRefClass *
y_WeakRef_type (y_Runtime * rt)
{
    y_GET_OR_CREATE_SUBTYPE (rt, weak_ref_type_name, y_WeakRefClass,
            y_Object_type, y_WeakRef_init_type, weak_ref_class);
}
