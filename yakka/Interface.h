#ifndef YAKKA_INTERFACE_H_
#define YAKKA_INTERFACE_H_

/** @defgroup Interface Interface
 *
 * Interfaces are declarations of functionality, for which classes may provide 
 * an implementation.
 *
 * Interfaces are declared as a struct of method pointers, with corresponding 
 * "wrapper" functions that call those methods.  The typical procedure is that 
 * the wrapper, which is invoked on an object instance and possibly with other 
 * arguments, will begin by fetching the implementation provided by the object.  
 * The implementation will be an instance of the interface struct (vtable) 
 * containing function pointers provided by that object's class.  The wrapper 
 * can then invoke the appropriate function, passing all relevant arguments.  
 * If the object's class does not implement the interface, some default action 
 * should be performed (such as raising an error or returning a default value).
 *
 * @{
 */

/**
 * Structure to contain an interface specification.
 *
 * This structure is used by classes to specify that they provide an 
 * implementation (as a vtable) of the interface identified by name.
 */
typedef struct y_InterfaceSpec {
    /** The name of the interface that is implemented */
    char * name;
    /** The implementation of the interface, as a vtable */
    void * vtable;
} y_InterfaceSpec;

/**
 * Packed set of interfaces.
 *
 * This structure is used internally by classes that implement an interface.
 */
typedef struct y_Interfaces {
    /**
     * Array of interface vtables, indexed by interface identifier.  Sparse: 
     * any unimplemented interfaces will be NULL.
     */
    void ** vtables;
    /**
     * The size of the interfaces member, as a number of elements.
     */
    int     size;
} y_Interfaces;

/**
 * Retrieve the implementation of an interface from an object instance.
 *
 * @param  instance  An instance object that may implement the interface.
 * @param  iface_type  The struct type of the interface.
 * @param  iface_name  The string name of the interface.
 * @param  iface_id  The name of the variable that contains the interface ID.
 * @return  The vtable via which the instance's type implements the interface, 
 * or NULL if it does not.
 */
#define y_GET_INSTANCE_VTABLE(instance, iface_type, iface_name, iface_id)   \
    ( (instance && (iface_id ||                                             \
                (iface_id =                                                 \
                 y_Runtime_get_interface_id (y_get_runtime (instance),      \
                     iface_name)))) ?                                       \
      (iface_type *)y_get_implementation (instance, iface_id) : NULL )

/** @} */
#endif
