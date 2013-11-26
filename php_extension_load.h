#ifndef PHP_EXTENSION_LOAD_H
#define PHP_EXTENSION_LOAD_H

#define EXTENSION_LOAD_EXT_VERSION "0.1.0"

extern zend_module_entry extension_load_module_entry;
#define phpext_extension_load_ptr &extension_load_module_entry

#ifdef PHP_WIN32
#    define PHP_EXTENSION_LOAD_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#    define PHP_EXTENSION_LOAD_API __attribute__ ((visibility("default")))
#else
#    define PHP_EXTENSION_LOAD_API
#endif

#ifdef ZTS
#    include "TSRM.h"
#endif

ZEND_BEGIN_MODULE_GLOBALS(extension_load)
    char *dir;
ZEND_END_MODULE_GLOBALS(extension_load)

#ifdef ZTS
#    define EXTENSION_LOAD_G(v) TSRMG(extension_load_globals_id, zend_extension_load_globals *, v)
#else
#    define EXTENSION_LOAD_G(v) (extension_load_globals.v)
#endif

#endif  /* PHP_EXTENSION_LOAD_H */
