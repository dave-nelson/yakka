#include "Error-protected.h"
#include <apr_strings.h>

#define ERRORSTR_SIZE 256

static const char * error_type_name = "y_ErrorClass";
static y_ErrorClass * error_class;

void
y_Error_throw (y_Runtime * rt, y_Error ** error, 
        const char * file, int lineno, int code, const char * description)
{
    y_Error * new_error = NULL;
    y_ErrorProtected * protect = NULL;
    apr_pool_t * pool = NULL;

    if ( ! error ) {
        /* Caller is ignoring errors -- do nothing */
        return;
    }

    new_error = y_create (rt, y_Error_type (rt), NULL);
    if ( new_error ) {
        pool = y_OBJECT_PROTECTED (new_error)->pool;
        protect = y_ERROR_PROTECTED (new_error);
    }
    else {
        return;  /* No space provided: caller ignoring errors */
    }
    assert (pool);

    if ( file ) {
        protect->file = apr_pstrdup (pool, file);
    }
    protect->lineno = lineno;
    protect->code = code;
    if ( description ) {
        protect->description = apr_pstrdup (pool, description);
    }
    if ( * error ) {
        /* Replace existing error with this one */
        protect->cause = *error;
    }
    *error = new_error;
}

bool
y_Error_throw_apr (y_Runtime * rt, y_Error ** error,
        const char * file, int lineno, apr_status_t code)
{
    bool failure = false;

    if ( code != APR_SUCCESS ) {
        failure = true;
        if ( error ) {
            char buf[ERRORSTR_SIZE];
            apr_strerror (code, buf, sizeof(buf));
            y_Error_throw (rt, error, file, lineno, (int)code, buf);
        }
    }
    return failure;
}

const char *
y_Error_get_file (void * self)
{
    y_Error * error = y_ERROR (self);
    return ( error ? y_ERROR_PROTECTED (error)->file : NULL );
}

int
y_Error_get_lineno (void * self)
{
    y_Error * error = y_ERROR (self);
    return ( error ? y_ERROR_PROTECTED(error)->lineno : 0 );
}

int
y_Error_get_code (void * self)
{
    y_Error * error = y_ERROR (self);
    return ( error ? y_ERROR_PROTECTED (error)->code : 0 );
}

const char *
y_Error_get_description (void * self)
{
    y_Error * error = y_ERROR (self);
    return ( error ? y_ERROR_PROTECTED (error)->description : NULL );
}

y_Error *
y_Error_get_cause (void * self)
{
    y_Error * error = y_ERROR (self);
    return ( error ? y_ERROR_PROTECTED (error)->cause : NULL );
}

void
y_Error_clear (void * self, bool unref_objects)
{
    y_Error * error = y_ERROR (self);
    y_ErrorProtected * protect = y_ERROR_PROTECTED (error);

    if ( protect ) {
        protect->code = 0;
        protect->description = NULL;  /* Allocated from pool: throw away */
        if ( protect->cause ) {
            y_unref (protect->cause);
            protect->cause = NULL;
        }
    }
}

void
y_Error_init_type (y_Runtime * rt, void * type, void * super_type)
{
    y_init_type (
            rt,
            type,
            super_type,
            error_type_name,
            sizeof (y_ErrorClass),
            sizeof (y_Error),
            sizeof (y_ErrorProtected),
            NULL,
            NULL,
            y_Error_clear
            );
}

void *
y_Error_type (y_Runtime * rt)
{
    y_GET_OR_CREATE_SUBTYPE (rt, error_type_name, y_ErrorClass,
            y_Object_type, y_Error_init_type, error_class);
}
