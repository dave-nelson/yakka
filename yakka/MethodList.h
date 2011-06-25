#ifndef YAKKA_METHOD_LIST_H_
#define YAKKA_METHOD_LIST_H_

#include <stdlib.h>
#include "Runtime.h"

typedef struct y_Method_ {
    void * type;
    void * exec;
} y_Method_;

typedef struct y_MethodList {
    size_t      size;
    y_Method_ * methods;
} y_MethodList;

y_MethodList * y_MethodList_extend (y_Runtime * rt, y_MethodList * existing,
        void * type, void * exec);

#endif
