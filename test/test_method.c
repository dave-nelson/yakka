/*
 * Yakka provides the facility to create lists of methods that may be executed 
 * in either direction.  This is used most notably in the Object class, where 
 * certain fundamental operations (initialise, assign and clear) are executed 
 * for a type and all its sub-types.
 *
 * This test suite checks that lists of methods can be created (from scratch) 
 * and executed in either direction with appropriate results.
 */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <yakka/Yakka.h>
#include <yakka/MethodList.h>


/* Definition of a method list for methods that accept an integer argument and 
 * return void. */
typedef struct TestMethodList {
    size_t  size;
    struct TestMethod {
        void     * type;
        void    (* exec) (int a_number);
    } * methods;
} TestMethodList;

y_Runtime * rt;

#define TEST_OUTPUT_SIZE  2
int output[TEST_OUTPUT_SIZE] = { 0, };
#define TEST_FIRST_METHOD_EXPECTED  1
#define TEST_SECOND_METHOD_EXPECTED  2

/*
 * Pointless method that simply writes a value to the specified position in the 
 * output array.
 */
void
first_method (int pos)
{
    assert ( pos >= 0 );
    assert ( pos < TEST_OUTPUT_SIZE );
    output[pos] = TEST_FIRST_METHOD_EXPECTED;
}

/*
 * (Similar to @ref first_method, but different value.)
 */
void
second_method (int pos)
{
    assert ( pos >= 0 );
    assert ( pos < TEST_OUTPUT_SIZE );
    output[pos] = TEST_SECOND_METHOD_EXPECTED;
}

/*
 * Execute a method list "forwards" (i.e. from original definition through to 
 * latest definition).
 */
void
forwards (TestMethodList * methods)
{
    if ( methods ) {
        assert (methods->size <= TEST_OUTPUT_SIZE);
        int i;
        for ( i = 0; i < methods->size; i++ ) {
            struct TestMethod method = methods->methods[i];
            method.exec (i);
        }
    }
}

/*
 * Execute a method list "backwards" (i.e. from latest definition back to 
 * original definition).
 */
void
backwards (TestMethodList * methods)
{
    if ( methods ) {
        assert (methods->size <= TEST_OUTPUT_SIZE);
        int i;
        for ( i = 0; i < methods->size; i++ ) {
            struct TestMethod method = methods->methods[methods->size - i - 1];
            method.exec (i);
        }
    }
}

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
test_new_method_list ()
{
    printf ("Test the creation of a new method list (%d)\n", __LINE__);
    /* Test that a method list can be created from scratch (not from 
     * existing) */
    TestMethodList * methods = (TestMethodList *)y_MethodList_extend (
            rt,
            NULL,
            NULL,
            first_method
            );
    assert (methods);
    assert (methods->size == 1);

    /* Since there's only one method, executing forwards or backwards should 
     * give the same result */
    int i;
    for ( i = 0; i < TEST_OUTPUT_SIZE; i++ ) {
        output[i] = 0;
    }
    forwards (methods);
    assert (output[0] == TEST_FIRST_METHOD_EXPECTED);
    assert (output[1] == 0);

    for ( i = 0; i < TEST_OUTPUT_SIZE; i++ ) {
        output[i] = 0;
    }
    backwards (methods);
    assert (output[0] == TEST_FIRST_METHOD_EXPECTED);
    assert (output[1] == 0);
}

void
test_extend_method_list ()
{
    printf ("Testing the extension of a method list, "
            "and executing it in both directions (%d)\n", __LINE__);
    /* Create the method list, then extend it */
    TestMethodList * methods = (TestMethodList *)y_MethodList_extend (
            rt,
            NULL,
            NULL,
            first_method
            );
    methods = (TestMethodList *)y_MethodList_extend (
            rt,
            (y_MethodList *)methods,
            NULL,
            second_method
            );
    int i;
    for ( i = 0; i < TEST_OUTPUT_SIZE; i++ ) {
        output[i] = 0;
    }
    forwards (methods);
    assert (output[0] == TEST_FIRST_METHOD_EXPECTED);
    assert (output[1] == TEST_SECOND_METHOD_EXPECTED);

    for ( i = 0; i < TEST_OUTPUT_SIZE; i++ ) {
        output[i] = 0;
    }
    backwards (methods);
    assert (output[0] == TEST_SECOND_METHOD_EXPECTED);
    assert (output[1] == TEST_FIRST_METHOD_EXPECTED);
}

int
main ()
{
    setup ();

    test_new_method_list ();
    test_extend_method_list ();

    teardown ();
    return 0;
}

#undef TEST_SECOND_METHOD_EXPECTED
#undef TEST_FIRST_METHOD_EXPECTED
#undef TEST_OUTPUT_SIZE

