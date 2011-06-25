#ifndef YAKKA_ERROR_PROTECTED_H_
#define YAKKA_ERROR_PROTECTED_H_

/** 
 * @addtogroup Error
 * @{
 *      @defgroup ErrorProtected Protected members of Error
 *      @{
 */

#include "Error.h"
#include "Object-protected.h"

typedef struct y_ErrorClass {
    y_ObjectClass object;
} y_ErrorClass;

typedef struct y_ErrorProtected {
    y_ObjectProtected   object;
    char              * file;
    int                 lineno;
    int                 code;
    char              * description;
    y_Error           * cause;
} y_ErrorProtected;
#define y_ERROR_PROTECTED(self)   \
    ( (y_ErrorProtected *)y_OBJECT_PROTECTED(self) )

/** @}@} */
#endif
