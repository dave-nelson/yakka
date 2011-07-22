/**
 * Test suite: member variables.
 *
 * Tests various operations on member variables of objects, including setting 
 * members on construction and setting members individually after construction.  
 * Also checks that member variables are set correctly by object copying and 
 * assignment.
 */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <apr_strings.h>
#include <yakka/Yakka.h>
#include <test/ootest/Alpha.h>
#include <test/ootest/Delta.h>

y_Runtime * rt;

struct test_members_fixture {
    int          a;
    const char * b;
    Gamma      * c;
    const char * d;
} fixtures[] = {
    { 1, "A string", NULL, "Another string" },
    { 0, }
};

void setup ()
{
    apr_status_t apr_status;

    apr_status = apr_initialize ();
    if ( apr_status != APR_SUCCESS )
        abort ();

    rt = y_Runtime_new (NULL, NULL, 1024, true);
    assert (rt);

    y_Error * error = NULL;
    Gamma * gamma = Gamma_new (rt, &error);
    assert (gamma);
    fixtures[0].c = gamma;

}

void
teardown ()
{
    y_unref (fixtures[0].c);
    y_Runtime_destroy (rt);
    apr_terminate ();
}

bool
strings_equal (const char * cmp1, const char * cmp2)
{
    if ( cmp1 && cmp2 ) {
        return (! apr_strnatcmp (cmp1, cmp2));
    }
    else {
        if ( cmp1 == cmp2 ) {
            return true;
        }
        else {
            return false;
        }
    }
}

void
check_delta (Delta * delta, int a, const char * b, Gamma * c, 
        const char * d)
{
    Alpha * alpha = ALPHA (delta);

    assert (alpha);
    assert (a == alpha->a);
    assert (strings_equal (b, Delta_get_b (delta)));
    assert (c == Delta_get_c (delta));
    assert (strings_equal (d, Delta_get_d (delta)));
}

void
test_set_on_construction ()
{
    printf ("Test setting member variables via constructor arguments (%d)\n",
            __LINE__);

    struct test_members_fixture fixture = fixtures[0];
    y_Error * error = NULL;
    Delta * delta = Delta_new (rt, fixture.a, fixture.b, fixture.c, fixture.d,
            &error);

    assert (! error);
    assert (delta);
    check_delta (delta, fixture.a, fixture.b, fixture.c, fixture.d);

    y_unref (delta);
}

void
test_set_individually ()
{
    printf ("Test setting member variables individually "
            "after construction (%d)\n", __LINE__);

    struct test_members_fixture fixture = fixtures[0];
    y_Error * error = NULL;
    Delta * delta = Delta_new (rt, 0, NULL, NULL, NULL, &error);
    Alpha * alpha = ALPHA (delta);

    assert (! error);
    assert (delta);
    alpha->a = fixture.a;
    Delta_set_b (delta, fixture.b);
    Delta_set_c (delta, fixture.c);
    Delta_set_d (delta, fixture.d);

    check_delta (delta, fixture.a, fixture.b, fixture.c, fixture.d);

    y_unref (delta);
}

void
test_clear ()
{
    printf ("Test that clearing an instance resets all its members (%d)\n",
            __LINE__);

    struct test_members_fixture fixture = fixtures[0];
    y_Error * error = NULL;
    Delta * delta = Delta_new (rt, fixture.a, fixture.b, fixture.c, fixture.d, 
            &error);

    assert (! error);
    assert (delta);
    check_delta (delta, fixture.a, fixture.b, fixture.c, fixture.d);
    y_clear (delta);
    check_delta (delta, 0, NULL, NULL, NULL);

    y_unref (delta);
}

void
test_copy ()
{
    printf ("Test copying an object with blank member variables (%d)\n",
            __LINE__);

    y_Error * error = NULL;
    Delta * delta = Delta_new (rt, 0, NULL, NULL, NULL, 
            &error);
    Delta * delta_copy = y_copy (delta, &error);

    assert (! error);
    assert (delta);
    assert (delta_copy);
    check_delta (delta_copy, 0, NULL, NULL, NULL);
    y_unref (delta_copy);
    y_unref (delta);
    y_unref (error);

    printf ("Test copying an object with member values set (%d)\n", __LINE__);

    struct test_members_fixture fixture = fixtures[0];
    error = NULL;
    delta = Delta_new (rt, fixture.a, fixture.b, fixture.c, fixture.d, 
            &error);
    delta_copy = y_copy (delta, &error);

    assert (! error);
    assert (delta);
    assert (delta_copy);
    check_delta (delta_copy, fixture.a, fixture.b, fixture.c, fixture.d);
    y_unref (delta_copy);
    y_unref (delta);
}

void
test_assign ()
{
    printf ("Test assigning an object from another (%d)\n", __LINE__);

    y_Error * error = NULL;
    struct test_members_fixture fixture = fixtures[0];
    Delta * delta1 = Delta_new (rt, fixture.a, fixture.b, fixture.c, fixture.d, 
            &error);
    Delta * delta2 = Delta_new (rt, 0, NULL, NULL, NULL, 
            &error);

    assert (! error);
    assert (delta1);
    assert (delta2);
    check_delta (delta2, 0, NULL, NULL, NULL);
    y_assign (delta2, delta1, &error);
    check_delta (delta2, fixture.a, fixture.b, fixture.c, fixture.d);
    assert (! error);
    y_unref (delta2);
    y_unref (delta1);
}

int main ()
{
    setup ();

    test_set_on_construction ();
    test_set_individually ();
    test_clear ();
    test_copy ();
    test_assign ();

    teardown ();
    return 0;
}
