#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <yakka/Yakka.h>
#include <test/ootest/Alpha.h>

y_Runtime * rt;

void setup ()
{
    apr_status_t apr_status;

    apr_status = apr_initialize ();
    if ( apr_status != APR_SUCCESS )
        abort ();

    rt = y_Runtime_new (NULL, NULL, 1024, NULL);
    assert (rt);
}

void
teardown ()
{
    y_Runtime_destroy (rt);
    apr_terminate ();
}

void
test_simple_object ()
{
    printf ("Test creating a simple object (%d)\n", __LINE__);
    
    y_Error * error = NULL;
    Alpha * alpha = Alpha_new (rt, 42, &error);

    assert (alpha);

    y_unref (alpha);
}

int
main ()
{
    printf ("Object tests: start (%s)\n\n", __FILE__);
    setup ();

    test_simple_object ();

    teardown ();
    printf ("\nObject tests: end\n");

    return 0;
}
