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

/**
 * The class structure for the type Error.
 */
typedef struct y_ErrorClass {
    /** The super class structure: Object. */
    y_ObjectClass object;
} y_ErrorClass;

/**
 * The protected instance structure for the type Error.
 */
typedef struct y_ErrorProtected {
    /** The super class protected instance structure. */
    y_ObjectProtected   object;
    /** The name of the source file that originated the error. */
    char              * file;
    /** The line number within the source file where the error originated. */
    int                 lineno;
    /** An application-specific error code number. */
    int                 code;
    /** A human-readable description of the error. */
    char              * description;
    /** An error that occurred previously, which caused this error (NULL if not 
     * applicable). */
    y_Error           * cause;
} y_ErrorProtected;

/** Convenience macro to insert the protected structure of an instance, cast as 
 * @ref y_ErrorProtected. */
#define y_ERROR_PROTECTED(self)   \
    ( (y_ErrorProtected *)y_OBJECT_PROTECTED(self) )

/**
 * @}
 * @}
 */
#endif
