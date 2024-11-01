/*
    +--------------------------------------------------------------------+
    | PECL :: win32ps                                                    |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted provided that the conditions mentioned |
    | in the accompanying LICENSE file are met.                          |
    +--------------------------------------------------------------------+
    | Copyright (c) 2005, Michael Wallner <mike@php.net>
    +--------------------------------------------------------------------+
*/

/* php_win32ps.h - Header file for the win32ps PHP extension */

#ifndef PHP_WIN32PS_H
#define PHP_WIN32PS_H

#include "php.h"

/* Define the extension's version and name */
#define PHP_WIN32PS_VERSION "2024.10.20"
#define PHP_WIN32PS_EXTNAME "win32ps"

/* Define export/import macros */
#ifdef PHP_WIN32PS_EXPORTS
#define PHP_WIN32PS_API __declspec(dllexport)
#else
#define PHP_WIN32PS_API __declspec(dllimport)
#endif

/* Define constants and error flags */
#define PHP_WIN32PS_MAXPROC 256

/* Error flags */
#define PHP_WIN32PS_NOTHING            0
#define PHP_WIN32PS_PID                0x01
#define PHP_WIN32PS_OPEN_HANDLE        0x02
#define PHP_WIN32PS_FILE_NAME          0x04
#define PHP_WIN32PS_MEM_INFO           0x08
#define PHP_WIN32PS_PROC_TIMES         0x10

/* Combined error flags for convenience */
#define PHP_WIN32PS_HANDLE_OPS \
    (PHP_WIN32PS_FILE_NAME | PHP_WIN32PS_MEM_INFO | PHP_WIN32PS_PROC_TIMES)
#define PHP_WIN32PS_ALL \
    (PHP_WIN32PS_PID | PHP_WIN32PS_OPEN_HANDLE | PHP_WIN32PS_HANDLE_OPS)

/* Declare module lifecycle functions */
PHP_MINIT_FUNCTION(win32ps);
PHP_MSHUTDOWN_FUNCTION(win32ps);
PHP_RINIT_FUNCTION(win32ps);
PHP_RSHUTDOWN_FUNCTION(win32ps);
PHP_MINFO_FUNCTION(win32ps);

/* Declare internal functions */
PHP_WIN32PS_API int php_win32ps_procinfo(DWORD proc, zval* array, int error_flags);
PHP_WIN32PS_API void php_win32ps_meminfo(zval* array);

/* Declare the PHP functions exposed by this extension */
PHP_FUNCTION(win32_ps_list_procs);
PHP_FUNCTION(win32_ps_stat_proc);
PHP_FUNCTION(win32_ps_stat_mem);

/* Declare the module entry externally */
extern zend_module_entry win32ps_module_entry;
#define phpext_win32ps_ptr &win32ps_module_entry

#endif /* PHP_WIN32PS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
