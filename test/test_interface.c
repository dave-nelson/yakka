#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <yakka/Yakka.h>
#include "ootest/Alpha.h"
#include "ootest/Beta.h"
#include "ootest/Gamma.h"

y_Runtime * rt;

void setup ()
{
    apr_status_t apr_status;

    apr_status = apr_initialize ();
    if ( apr_status != APR_SUCCESS )
        abort ();

    rt = y_Runtime_new (NULL, NULL, 1024, true);
    assert (rt);
}

void
teardown ()
{
    y_Runtime_destroy (rt);
    apr_terminate ();
}

void
test_interface_id ()
{
    printf ("Test allocation of interface identifiers (%d)\n", __LINE__);
    
    char * fooName = "foo";
    char * barName = "bar";

    /* Interface identifiers are always greater than 0 */
    assert (y_Runtime_get_interface_id (rt, fooName) > 0);
    /* Getting an interface identifier twice, by the same name, should have the 
     * same result */
    assert (y_Runtime_get_interface_id (rt, fooName) ==
            y_Runtime_get_interface_id (rt, fooName) );
    /* Interface identifiers should differ for different names */
    assert (y_Runtime_get_interface_id (rt, fooName) !=
            y_Runtime_get_interface_id (rt, barName) );
}

void
test_interface_get_implementation ()
{
    printf ("Test retrieving the implementation of an interface (%d)\n",
            __LINE__);

    Alpha * alpha = Alpha_new (rt, 1, NULL);
    Gamma * gamma = Gamma_new (rt, NULL);
    int iface_id = y_Runtime_get_interface_id (rt, Beta_name);

    assert (alpha);
    assert (gamma);

    /* Alpha doesn't implement the interface */
    assert ( ! y_get_implementation (alpha, iface_id) );
    assert ( ! y_get_implementation_by_name (alpha, Beta_name));

    /* Gamma does */
    assert ( y_get_implementation (gamma, iface_id) );
    assert ( y_get_implementation (gamma, iface_id) ==
            y_get_implementation_by_name (gamma, Beta_name) );

    y_unref (alpha);
    y_unref (gamma);
}

void
test_interface_use ()
{
    printf ("Test using the implementation of an interface (%d)\n", __LINE__);

    Alpha * alpha = Alpha_new (rt, 1, NULL);
    Gamma * gamma = Gamma_new (rt, NULL);

    assert (alpha);
    assert (gamma);

    /* Alpha doesn't implement the interface: should return default */
    assert (Beta_do_action (alpha) == Beta_default);
    /* Gamma does: should return the value of Gamma_default */
    assert (Beta_do_action (gamma) == Gamma_default);

    y_unref (alpha);
    y_unref (gamma);
}

int
main ()
{
    setup ();

    test_interface_id ();
    test_interface_get_implementation ();
    test_interface_use ();

    teardown ();
    return 0;
}
