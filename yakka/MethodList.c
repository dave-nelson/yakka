#include "MethodList.h"

#define APR_WANT_MEMFUNC
#include <apr_want.h>

y_MethodList *
y_MethodList_extend (y_Runtime * rt, y_MethodList * existing,
        void * type, void * exec)
{
    y_MethodList * new_list = NULL;
    y_Method_ new_method = { type, exec };
    apr_pool_t * pool = NULL;
    
    pool = y_Runtime_get_global_pool (rt);
    new_list = apr_pcalloc (pool, sizeof (y_MethodList));
    new_list->size = (existing ? existing->size : 0) + 1;
    new_list->methods = apr_pcalloc (pool, new_list->size * sizeof (y_Method_));
    if ( existing ) {
        memcpy (new_list->methods, existing->methods,
                existing->size * sizeof (y_Method_));
    }
    new_list->methods[new_list->size - 1] = new_method;

    return new_list;
}
