#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend.h"
#include "zend_extensions.h"

#include "php_extension_load.h"

ZEND_DECLARE_MODULE_GLOBALS(extension_load)

ZEND_INI_BEGIN()
STD_ZEND_INI_ENTRY("extension_load.dir", (char *)NULL,
                   ZEND_INI_SYSTEM, OnUpdateString, dir,
                   zend_extension_load_globals, extension_load_globals)
ZEND_INI_END()

ZEND_BEGIN_ARG_INFO_EX(arginfo_extension_load, 0, 0, 1)
    ZEND_ARG_INFO(0, filepath)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(extension_load)
{
    char *libpath = NULL, *fname = NULL, *dirname = NULL;
    char *extension_dir = NULL;
    char *p = NULL;
    char *filepath;
    int filepath_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s",
                              &filepath, &filepath_len) == FAILURE) {
        return;
    }

    if (filepath_len >= MAXPATHLEN) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING,
                         "File path exceeds the maximum allowed "
                         "length of %d characters", MAXPATHLEN);
        RETURN_FALSE;
    }
    if (!filepath || filepath_len <= 0) {
        RETURN_FALSE;
    }

    libpath = estrdup(filepath);

    if ((p = strrchr(libpath, '/')) != NULL ||
        (p = strrchr(libpath, DEFAULT_SLASH)) != NULL) {
        int pos = p - libpath;
        libpath[pos++] = '\0';
        dirname = libpath;
        fname = libpath + pos;
    } else {
        fname = libpath;
    }

    if (dirname) {
        extension_dir = PG(extension_dir);
        PG(extension_dir) = dirname;
    }

    php_dl(fname, MODULE_TEMPORARY, return_value, 0 TSRMLS_CC);
    if (Z_LVAL_P(return_value) == 1) {
        EG(full_tables_cleanup) = 1;
    }

    if (extension_dir) {
        PG(extension_dir) = extension_dir;
    }

    efree(libpath);
}

static zend_function_entry extension_load_functions[] = {
    ZEND_FE(extension_load, arginfo_extension_load)
    ZEND_FE_END
};

static void
extension_load_init_globals(zend_extension_load_globals *extension_load_globals)
{
    extension_load_globals->dir = NULL;
}

ZEND_MINIT_FUNCTION(extension_load)
{
    char *dirname = NULL;

    ZEND_INIT_MODULE_GLOBALS(extension_load, extension_load_init_globals, NULL);
    REGISTER_INI_ENTRIES();

    dirname = EXTENSION_LOAD_G(dir);
    if (dirname) {
        char *extension_dir = NULL;
        DIR* dir;
        struct dirent* dp;
        int full_tables_cleanup = 0;

        extension_dir = PG(extension_dir);
        PG(extension_dir) = dirname;

        if ((dir = opendir(dirname)) != NULL) {
            while ((dp = readdir(dir)) != NULL) {
                size_t len = strlen(dp->d_name);
                if (len <= 3) {
                    continue;
                }
                if (strncasecmp(dp->d_name + len - 3, ".so", 3) == 0) {
                    zval zv;
                    php_dl(dp->d_name, MODULE_TEMPORARY, &zv, 0 TSRMLS_CC);
                    if (Z_LVAL(zv) == 1) {
                        full_tables_cleanup = 1;
                    }
                }
            }
            closedir(dir);
        }

        if (full_tables_cleanup) {
            EG(full_tables_cleanup) = 1;
        }

        if (extension_dir) {
            PG(extension_dir) = extension_dir;
        }
    }

    return SUCCESS;
}

ZEND_MSHUTDOWN_FUNCTION(extension_load)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

ZEND_MINFO_FUNCTION(extension_load)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "Extension load support", "enabled");
    php_info_print_table_row(2, "Extension Version", EXTENSION_LOAD_EXT_VERSION);
    php_info_print_table_end();
}

zend_module_entry extension_load_module_entry = {
    STANDARD_MODULE_HEADER,
    "extension_load",
    extension_load_functions,
    ZEND_MINIT(extension_load),
    ZEND_MSHUTDOWN(extension_load),
    NULL,
    NULL,
    ZEND_MINFO(extension_load),
    EXTENSION_LOAD_EXT_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#if COMPILE_DL_EXTENSION_LOAD
ZEND_GET_MODULE(extension_load)
#endif
