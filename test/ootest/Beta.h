#ifndef BETA_H_
#define BETA_H_

#include <yakka/Yakka.h>
#include <yakka/Interface.h>

/**
 * Example of an interface.
 */
static char * Beta_name = "Beta";
static const int Beta_default = 0;

/**
 * Vtable for interface.
 */
typedef struct Beta {
    int (*do_action) (void * self);
} Beta;

/**
 * Create an implementation of the Beta interface from the provided function pointers.
 */
Beta * Beta_implement (y_Runtime * rt,
    int (*do_action) (void * self)
    );

/**
 * Accessors for interface methods.
 */
int Beta_do_action (void * self);

#endif
