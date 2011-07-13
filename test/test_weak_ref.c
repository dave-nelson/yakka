#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <yakka/Yakka.h>

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
test_weak_ref_deref ()
{
    printf ("Test creating a weak reference and using it to get "
            "the referent (%d)\n", __LINE__);
    /* Create some object */
    y_Error * error = NULL;
    y_Object * obj = y_Object_new (rt, &error);
    assert (obj);
    assert (! error);

    y_WeakRef * weakref = y_weak_ref (obj);
    assert (weakref);

    /* Weak ref should enable the same object to be retrieved */
    y_Object * copy = y_WeakRef_deref (weakref);
    assert (copy == obj);
    
    y_unref (copy);
    y_unref (obj);
    y_unref (weakref);
}

void
test_weak_ref_is_set ()
{
    printf ("Test whether a weak ref handles its referent going away (%d)\n",
            __LINE__);
    /* Create some object */
    y_Error * error = NULL;
    y_Object * obj = y_Object_new (rt, &error);
    assert (obj);
    assert (! error);

    y_WeakRef * weakref = y_weak_ref (obj);
    assert (weakref);
    assert (y_WeakRef_is_set (weakref));

    y_unref (obj);
    /* Original object should now be gone */
    assert (! y_WeakRef_is_set (weakref));
    assert (! y_WeakRef_deref (weakref));  /* should return NULL */

    y_unref (weakref);
}

void
test_weak_ref_multiple ()
{
    printf ("Test that multiple references/dereferences maintain the object "
            "refcount correctly (%d)\n", __LINE__);
    /* Create some object */
    y_Error * error = NULL;
    y_Object * obj = y_Object_new (rt, &error);
    assert (obj);
    assert (! error);

    y_WeakRef * weakref = y_weak_ref (obj);
    assert (weakref);
    assert (y_WeakRef_is_set (weakref));

    y_Object * copy = y_WeakRef_deref (weakref);
    assert (y_WeakRef_is_set (weakref));
    
    /* After the first unref the weak ref should still be set, but not after 
     * the second */
    y_unref (copy);
    assert (y_WeakRef_is_set (weakref));
    y_unref (obj);
    assert (! y_WeakRef_is_set (weakref));

    y_unref (weakref);
}

int
main ()
{
    setup ();

    test_weak_ref_deref ();
    test_weak_ref_is_set ();
    test_weak_ref_multiple ();

    teardown ();
    return 0;
}
