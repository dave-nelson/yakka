#include <assert.h>
#include <stdio.h>
#include <apr_strings.h>
#include "yakka/Yakka.h"

y_Runtime * rt;

struct test_error_fixture {
    int code;
    char * description;
} fixtures[] = {
    {  1, "First error" },
    {  2, "Second error" },
    { 42, "Yet another error" },
    {  0, }
};


void
setup ()
{
    apr_status_t apr_status;

    apr_status = apr_initialize ();
    if ( apr_status != APR_SUCCESS ) {
        assert (0);
    }

    rt = y_Runtime_new (NULL, NULL, 1024, NULL);
    assert (rt);
}

void
teardown ()
{
    y_Runtime_destroy (rt);
    apr_terminate ();
}

/**
 * Test the creation/acquisition of the Error class type.
 */
void
test_Error_type ()
{
    void * error_type1 = y_Error_type (rt);
    void * error_type2 = y_Error_type (rt);

    printf ("\tTest the creation/acquisition of the Error class type (%d)\n",
            __LINE__);

    /* Was type successfully created/acquired? */
    assert (error_type1);
    /* Subsequent call provides same result */
    assert (error_type1 == error_type2);
}

/**
 * Test that an Error can be created.
 */
void
test_Error_throw ()
{
    y_Error * error = NULL;

    printf ("\tTest throwing an error (%d)\n", __LINE__);

    y_Error_throw (rt, &error, __FILE__, __LINE__, 
            fixtures[0].code, fixtures[0].description);
    assert (error);

    assert (y_Error_get_file (error));
    assert (! apr_strnatcmp (__FILE__, y_Error_get_file (error)));
    assert (__LINE__ > y_Error_get_lineno (error));
    assert (fixtures[0].code == y_Error_get_code (error));
    assert (y_Error_get_description (error));
    assert (! apr_strnatcmp (fixtures[0].description, y_Error_get_description (error)));
    assert (! y_Error_get_cause (error));

    y_unref (error);
}

/**
 * Test ignore errors.
 *
 * When the caller to a method is not interested in receiving errors, they may 
 * pass NULL as the error storage.  In that case performing the steps of 
 * throwing an error is okay, but no action will be performed.
 *
 * This should work without segfaulting.  :)
 */
void
test_ignore_throw ()
{
    y_Error ** error = NULL;

    printf ("\tTest ignoring thrown errors (%d)\n", __LINE__);

    y_Error_throw (rt, error, __FILE__, __LINE__,
            1, "Some kind of error");
    assert (! error);
}

/**
 * Test that when errors are stacked that all errors are represented correctly.
 */
void
test_Error_cause ()
{
    y_Error * error = NULL;

    printf ("\tTest stacked errors (%d)\n", __LINE__);

    /* Throw all errors */
    int i = 0;
    while ( fixtures[i].code ) {
        assert (i < 100);
        y_Error_throw (rt, &error, __FILE__, __LINE__,
                fixtures[i].code, fixtures[i].description);
        i++;
    }
    assert (error);

    /* Check each error */
    y_Error * error_tmp = error;
    while ( error_tmp ) {
        i--;
        assert (! (i < 0));

        assert (y_Error_get_file (error_tmp));
        assert (! apr_strnatcmp (__FILE__, y_Error_get_file (error_tmp)));
        assert (__LINE__ > y_Error_get_lineno (error_tmp));
        assert (fixtures[i].code == y_Error_get_code (error_tmp));
        assert (y_Error_get_description (error_tmp));
        assert (! apr_strnatcmp (fixtures[i].description, y_Error_get_description (error_tmp)));

        error_tmp = y_Error_get_cause (error_tmp);
    }
    assert (i == 0);
  
    y_unref (error);
}

/**
 * (This is a fake APR method that always succeeds.)
 */
apr_status_t
faux_apr_good_method ()
{
    return APR_SUCCESS;
}

/**
 * (This is a fake APR method that always fails.)
 */
apr_status_t
faux_apr_bad_method ()
{
    return APR_EBADPATH;
}

/**
 * Test that an error originating from an APR method can be thrown as a Yakka error.
 *
 * Some APR methods indicate error by returning an apr_status_t value, which 
 * can be used to retrieve an error string.  To accommodate this pattern in 
 * Yakka, we detect whether an error has occurred and if so we throw a Yakka 
 * error with the code and description from APR.
 */
void
test_Error_throw_apr ()
{
    y_Error * error = NULL;
    char buf[256];

    printf ("\tTest throwing an error from an APR method (%d)\n", __LINE__);

    /* The "good" method always succeeds */
    assert (! y_Error_throw_apr (rt, &error, __FILE__, __LINE__,
                faux_apr_good_method ()));
    assert (! error);

    /* The "bad" method always fails with an APR error */
    assert (y_Error_throw_apr (rt, &error, __FILE__, __LINE__,
                faux_apr_bad_method ()));
    assert (error);
    assert (faux_apr_bad_method () == y_Error_get_code (error));
    /* Fetch the error description from APR and check */
    apr_strerror (faux_apr_bad_method (), buf, sizeof (buf));
    assert (! apr_strnatcmp (buf, y_Error_get_description (error)));

    y_unref (error);
}

int main ()
{
    setup ();

    printf ("Error tests: start (%s)\n", __FILE__);
    test_Error_type ();
    test_Error_throw ();
    test_Error_cause ();
    test_ignore_throw ();
    test_Error_throw_apr ();
    printf ("Error tests: end\n");

    teardown ();

    return 0;
}
