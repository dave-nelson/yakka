#include "Alpha-protected.h"

static char * alpha_type_name = "Alpha";
static AlphaClass * alpha_class = NULL;

Alpha *
Alpha_new (y_Runtime * rt, int a,
        y_Error ** error)
{
    Alpha * self = (Alpha *)y_create (rt, Alpha_type (rt), error);
    Alpha_set (self, a, error);
    return self;
}

void
Alpha_set (Alpha * self, int a, y_Error ** error)
{
    self->a = a;
}

void *
Alpha_assign (void * to, const void * from,
        y_Error ** error)
{
    Alpha * alpha_to = ALPHA (to);
    const Alpha * alpha_from = ALPHA (from);

    if ( alpha_to && alpha_from ) {
        alpha_to->a = alpha_from->a;
    }
    return to;
}

void
Alpha_clear (void * self, bool unref_objects)
{
    Alpha * alpha = ALPHA (self);

    if ( alpha ) {
        alpha->a = 0;
    }
}

void
Alpha_init_type (y_Runtime * rt, void * type, void * super_type)
{
    y_init_type (
            rt,
            type,
            super_type,
            alpha_type_name,
            sizeof (AlphaClass),
            sizeof (Alpha),
            sizeof (AlphaProtected),
            NULL,  /* no special init required */
            Alpha_assign,
            Alpha_clear
            );
}

AlphaClass *
Alpha_type (y_Runtime * rt)
{
    y_GET_OR_CREATE_SUBTYPE (rt, alpha_type_name, AlphaClass, y_Object_type,
            Alpha_init_type, alpha_class);
}
