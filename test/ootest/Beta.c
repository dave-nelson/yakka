#include "Beta.h"

static int beta_id = 0;

Beta *
Beta_implement (y_Runtime * rt,
    int (*do_action) (void * self)
    ) 
{
    Beta * beta = apr_pcalloc (y_Runtime_get_global_pool (rt),
            sizeof (Beta));
    beta->do_action = do_action;
    return beta;
}

int
Beta_do_action (void * self)
{
    Beta * vtable = y_GET_INSTANCE_VTABLE (self, Beta, Beta_name, beta_id);
    if ( vtable ) {
        return vtable->do_action (self);
    }
    else {
        return Beta_default;
    }
}
