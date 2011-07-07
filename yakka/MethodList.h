#ifndef YAKKA_METHOD_LIST_H_
#define YAKKA_METHOD_LIST_H_

/** @defgroup MethodList  Method list
 *
 * An extensible list of methods that can be executed on an instance, within 
 * the context of a particular type.
 * @{
 */

#include <stdlib.h>
#include "Runtime.h"

/**
 * Pointer to a method provided by a class.
 * 
 * Note that the method is typed.  This is to enable an instance to be blessed 
 * (@ref y_bless) to the appropriate type before the method is invoked.  This 
 * ensures that any methods are executed in their appropriate context, i.e. as 
 * if the instance were of that type.
 */
typedef struct y_Method_ {
    /** The type for which the method is defined. */
    void * type;
    /** Function pointer to the method to execute. */
    void * exec;
} y_Method_;

/**
 * A list of method pointers.
 */
typedef struct y_MethodList {
    /** Size (as a number of elements) of the method pointer list. */
    size_t      size;
    /** Array of method pointers @ref y_Method_. */
    y_Method_ * methods;
} y_MethodList;

/**
 * Extend a method list by adding a new method.
 *
 * Typically this function is used by sub-classes, to extend the method list of 
 * its super class by adding a new method.  The existing method list is copied 
 * and the new method is added to the end.
 *
 * @param  rt  The Yakka runtime.
 * @param  existing  The existing method list that is to be extended.  If NULL, 
 * a new method list will be created.
 * @param  type  The class type to which the method applies.
 * @param  exec  Function pointer to the method to be executed.
 * @return  A new method list, consisting of a copy of the existing method list 
 * (if applicable) with the new method appended.
 */
y_MethodList * y_MethodList_extend (y_Runtime * rt, y_MethodList * existing,
        void * type, void * exec);

/**
 * @}
 */
#endif
