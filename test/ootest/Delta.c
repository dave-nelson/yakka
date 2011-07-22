#include "Delta-protected.h"
#define APR_WANT_STRFUNC
#include <apr_want.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h> /* malloc and free */
#endif

static char * delta_type_name = "Delta";
static DeltaClass * delta_class = NULL;

typedef struct DeltaPrivate {
    char * d;
} DeltaPrivate;

Delta *
Delta_new (y_Runtime * rt, int a, const char * b, Gamma * c, const char * d,
        y_Error ** error)
{
    Delta * self = (Delta *)y_create (rt, Delta_type (rt), error);
    Delta_set (self, a, b, c, d, error);
    return self;
}

void
Delta_set (Delta * self, int a, const char * b, Gamma * c, const char * d,
        y_Error ** error)
{
    /* Super-class setter */
    Alpha_set ((Alpha *)self, a, error);
    /* Setters for members defined by this class */
    Delta_set_b (self, b);
    Delta_set_c (self, c);
    Delta_set_d (self, d);
}

/**
 * Setter for an explicitly managed member variable (in this case a string 
 * using malloc'd memory).  The variable is stored in the protected structure.
 */
void
Delta_set_b (void * self, const char * b)
{
    Delta * delta = DELTA (self);
    DeltaProtected * prot = DELTA_PROTECTED (delta);
    if ( prot ) {
        if ( prot->b ) {
            free (prot->b);
        }
        size_t len = 0;
        char * b_copy = NULL;
        if ( b && (len = strlen (b)) ) {
            b_copy = malloc (len + 1);
            strncpy (b_copy, b, len);
            b_copy[len] = '\0';
        }
        prot->b = b_copy;
    }
}

const char * 
Delta_get_b (void * self)
{
    Delta * delta = DELTA (self);
    DeltaProtected * prot = DELTA_PROTECTED (delta);
    if ( prot ) {
        return prot->b;
    }
    else {
        return NULL;
    }
}

/**
 * Setter for an object reference.
 *
 * The caller creates the reference and passes it.  We do not unref the 
 * reference because we are keeping it.  However if there is an existing 
 * reference, it should be unreffed because it will not be kept.
 */
void
Delta_set_c (void * self, Gamma * c)
{
    Delta * delta = DELTA (self);
    DeltaProtected * prot = DELTA_PROTECTED (delta);
    if ( prot ) {
        if ( prot->c ) {
            y_unref (prot->c);
        }
        prot->c = y_ref (c);
    }
}

/**
 * Getter for an object reference.
 *
 * Note that we ref the object because we are sharing it with some caller.  The 
 * caller should unref their copy once it is no longer in use.
 */
Gamma *
Delta_get_c (void * self)
{
    Delta * delta = DELTA (self);
    DeltaProtected * prot = DELTA_PROTECTED (delta);
    if ( prot ) {
        return prot->c;
    }
    else {
        return NULL;
    }
}

/**
 * Setter for an explicitly managed member variable (in this case a string 
 * using malloc'd memory).  Very similar to the other case above, except that 
 * this one is stored in the private structure.
 */
void
Delta_set_d (void * self, const char * d)
{
    Delta * delta = DELTA (self);
    DeltaProtected * prot = DELTA_PROTECTED (delta);
    if ( prot ) {
        DeltaPrivate * priv = prot->priv;
        if ( priv->d ) {
            free (priv->d);
        }
        size_t len = 0;
        char * d_copy = NULL;
        if ( d && (len = strlen (d)) ) {
            d_copy = malloc (len + 1);
            strncpy (d_copy, d, len);
            d_copy[len] = '\0';
        }
        priv->d = d_copy;
    }
}

const char * 
Delta_get_d (void * self)
{
    Delta * delta = DELTA (self);
    DeltaProtected * prot = DELTA_PROTECTED (delta);
    if ( prot ) {
        return prot->priv->d;
    }
    else {
        return NULL;
    }
}

/**
 * This class needs an init method because a private data structure needs to be 
 * allocated.
 */
void
Delta_init (void * self,
        y_Error ** error)
{
    Delta * delta = DELTA (self);
    DeltaProtected * prot = DELTA_PROTECTED (delta);
    y_ObjectProtected * obj_prot = (y_ObjectProtected *)prot;
    if ( prot ) {
       prot->priv = apr_pcalloc (obj_prot->pool, sizeof (DeltaPrivate));
    }
}

void *
Delta_assign (void * to, const void * from,
        y_Error ** error)
{
    Delta * delta_from = DELTA (from);
    Delta * delta_to = DELTA (to);
    if ( delta_from && delta_to ) {
        Delta_set_b (delta_to, Delta_get_b (delta_from));
        Delta_set_c (delta_to, Delta_get_c (delta_from));
        Delta_set_d (delta_to, Delta_get_d (delta_from));
    }
    return delta_to;
}

void
Delta_clear (void * self, bool unref_objects)
{
    Delta * delta = DELTA (self);
    if ( delta ) {
        Delta_set_b (delta, NULL);
        if ( unref_objects ) {
            Delta_set_c (delta, NULL);            
        }
        Delta_set_d (delta, NULL);
    }
}

void
Delta_init_type (y_Runtime * rt, void * type, void * super_type)
{
    y_init_type (
            rt,
            type,
            super_type,
            delta_type_name,
            sizeof (DeltaClass),
            sizeof (Delta),
            sizeof (DeltaProtected),
            Delta_init,
            Delta_assign,
            Delta_clear
            );
}

DeltaClass *
Delta_type (y_Runtime * rt)
{
    y_GET_OR_CREATE_SUBTYPE (rt, delta_type_name, DeltaClass, Alpha_type,
            Delta_init_type, delta_class);
}
