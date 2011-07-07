#ifndef YAKKA_ERROR_H_
#define YAKKA_ERROR_H_

/** \defgroup Error Error
 * 
 * Class representing an error or exception generated at run-time.
 * @{
 */

#include "Object.h"

struct y_Runtime;

/**
 * Get the type for Error.
 */
void * y_Error_type (struct y_Runtime * rt);

/**
 * Public instance structure for an Error.
 */
typedef struct y_Error {
    /** The super class structure for an Error instance: @ref y_Object. */
    y_Object object;
} y_Error;

/** Convenience macro to insert an instance, safely cast to @ref y_Error. */
#define y_ERROR(self) \
    y_SAFE_CAST_INSTANCE(self, y_Error_type, y_Error)

/**
 * Set an error, possibly appending to an existing one.
 *
 * If 'error' (i.e. y_Error **) is NULL, this indicates that the caller is 
 * ignoring errors, so no action is performed.  
 *
 * If '*error' is not NULL, then there is an existing error.  This method will 
 * create an error with that error as its cause.  This new error will be set 
 * into 'error'.  Reference counts will be updated as appropriate.
 *
 * If there is no existing error, a new error will be created and set into 
 * 'error'.
 */
void y_Error_throw (struct y_Runtime * rt, y_Error ** error,
        const char * file, int lineno, int code, const char * description);

/**
 * Set an error based on an Apache Portable Runtime error code.
 *
 * If the provided 'error' is not NULL and the 'code' is not APR_SUCCESS,
 * a new error will be created and set into 'error'.  Otherwise no action is 
 * required.
 *
 * @param  rt  Yakka runtime.
 * @param  error  Destination for an error.
 * @param  file  The source file where the error originated.
 * @param  lineno  The line in the source file where the error originated.
 * @param  code  APR status code, typically produced by an APR method call.
 * @return  True if an error was caught; false if no error
 */
bool y_Error_throw_apr (struct y_Runtime * rt, y_Error ** error,
        const char * file, int lineno, apr_status_t code);

/**
 * Get the filename from which the error originated.
 *
 * @param  self  An instance of @ref y_Error.
 * @return  The file name associated with the error.
 */
const char * y_Error_get_file (void * self);

/**
 * Get the line number of the source code from which the error originated.
 *
 * @param  self  An instance of @ref y_Error.
 * @return The line number of the source file where the error originated. 
 */
int y_Error_get_lineno (void * self);

/**
 * Get the code associated with an error.
 *
 * @param  self  An instance of @ref y_Error.
 * @return  The code associated with the error.
 */
int y_Error_get_code (void * self);

/**
 * Get the description associated with an error.
 *
 * @param  self  An instance of @ref y_Error.
 * @return  The description associated with the error.
 */
const char * y_Error_get_description (void * self);

/**
 * Get the cause of an error.
 *
 * @param  self  An instance of @ref y_Error.
 * @return  The cause of the error, or NULL if there was no cause.
 */
y_Error * y_Error_get_cause (void * self);

/** @} */
#endif
