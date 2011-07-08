#include "Gamma-protected.h"
#include "Beta.h"

static char * gamma_type_name = "Gamma";
static GammaClass * gamma_class = NULL;

int Beta_do_action_impl (void * self);

Gamma *
Gamma_new (y_Runtime * rt, y_Error ** error)
{
    Gamma * self = (Gamma *)y_create (rt, Gamma_type (rt), error);
    return self;
}

void
Gamma_init_type (y_Runtime * rt, void * type, void * super_type)
{
    y_init_type (
            rt,
            type,
            super_type,
            gamma_type_name,
            sizeof (GammaClass),
            sizeof (Gamma),
            sizeof (GammaProtected),
            NULL,
            NULL,
            NULL
            );
    y_ObjectClass * object_type = (y_ObjectClass *)type;

    Beta * beta_impl = Beta_implement (rt,
        Beta_do_action_impl
        );
    y_InterfaceSpec interface_specs[] = {
        { Beta_name, beta_impl},
        { NULL, }
    };
    object_type->interfaces = y_Runtime_pack_interfaces (rt, interface_specs);
}

int
Beta_do_action_impl (void * self)
{
    Gamma * gamma = GAMMA (self);
    return Gamma_default;
}

GammaClass *
Gamma_type (y_Runtime * rt)
{

    y_GET_OR_CREATE_SUBTYPE (rt, gamma_type_name, GammaClass, y_Object_type,
            Gamma_init_type, gamma_class);
}
