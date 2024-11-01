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

/* win32ps.c - Implementation of the win32ps PHP extension */

#include "php.h"
#pragma comment(lib, "php8ts.lib")

#include "ext/standard/info.h"
#include "php_win32ps.h"
#include "Zend/zend_config.w32.h"

#include <windows.h>
#include <psapi.h>

/* Declare the functions provided by this extension */
zend_function_entry win32ps_functions[] = {
    PHP_FE(win32_ps_list_procs,     NULL)
    PHP_FE(win32_ps_stat_proc,      NULL)
    PHP_FE(win32_ps_stat_mem,       NULL)
    PHP_FE_END
};

/* Define the module entry using standard macros */
zend_module_entry win32ps_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_WIN32PS_EXTNAME,
    win32ps_functions,
    PHP_MINIT(win32ps),
    PHP_MSHUTDOWN(win32ps),
    PHP_RINIT(win32ps),
    PHP_RSHUTDOWN(win32ps),
    PHP_MINFO(win32ps),
    PHP_WIN32PS_VERSION,
    STANDARD_MODULE_PROPERTIES
};

/* Export the module */
#ifdef COMPILE_DL_WIN32PS
ZEND_GET_MODULE(win32ps)
#endif

/* Define an error macro for consistent error handling */
#define php_win32ps_error(message, pid) \
    php_error_docref(NULL, E_WARNING, "Process Status Error (%lu): " message " (PID %lu)", GetLastError(), (ULONG)pid)

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(win32ps)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "Win32 Process Status Support", "enabled");
    php_info_print_table_row(2, "Extension Version", PHP_WIN32PS_VERSION);
    php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(win32ps)
{
    /* You can perform initialization tasks here if needed */
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(win32ps)
{
    /* You can perform shutdown tasks here if needed */
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(win32ps)
{
    /* You can perform request initialization tasks here if needed */
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
PHP_RSHUTDOWN_FUNCTION(win32ps)
{
    /* You can perform request shutdown tasks here if needed */
    return SUCCESS;
}
/* }}} */

/* {{{ php_win32ps_procinfo */
PHP_WIN32PS_API int php_win32ps_procinfo(DWORD proc, zval* return_array, int error_flags)
{
    char exe_path[MAX_PATH + 1] = { 0 };
    zval mem, tms;
    HANDLE hProcess;
    FILETIME creat, exit, kern, user;
    ULONGLONG creatx, kernx, userx;
    PROCESS_MEMORY_COUNTERS memory = { sizeof(PROCESS_MEMORY_COUNTERS) };

    if (proc == 0) {
        /* No info for the idle process */
        if (error_flags & PHP_WIN32PS_PID) {
            php_win32ps_error("Invalid process ID", proc);
        }
        return FAILURE;
    }

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, proc);
    if (!hProcess) {
        /* Unable to open process handle */
        if (error_flags & PHP_WIN32PS_OPEN_HANDLE) {
            php_win32ps_error("Could not open process handle", proc);
        }
        return FAILURE;
    }

    if (proc == 8) {
        strncpy_s(exe_path, sizeof(exe_path), "*SYSTEM*", _TRUNCATE);
    }
    else {
        if (!GetModuleFileNameExA(hProcess, NULL, exe_path, MAX_PATH)) {
            if (error_flags & PHP_WIN32PS_FILE_NAME) {
                php_win32ps_error("Could not determine executable path", proc);
            }
            CloseHandle(hProcess);
            return FAILURE;
        }
    }

    if (!GetProcessMemoryInfo(hProcess, &memory, sizeof(memory))) {
        if (error_flags & PHP_WIN32PS_MEM_INFO) {
            php_win32ps_error("Could not determine memory usage", proc);
        }
        CloseHandle(hProcess);
        return FAILURE;
    }

    if (!GetProcessTimes(hProcess, &creat, &exit, &kern, &user)) {
        if (error_flags & PHP_WIN32PS_PROC_TIMES) {
            php_win32ps_error("Could not determine process times", proc);
        }
        CloseHandle(hProcess);
        return FAILURE;
    }
    CloseHandle(hProcess);

    /* Populate the return array */
    array_init(return_array);
    add_assoc_long(return_array, "pid", (long)proc);

    /* Sanitize and add the executable path */
    if (strncmp(exe_path, "\\??\\", 4) == 0) {
        add_assoc_string(return_array, "exe", exe_path + 4, 1);
    }
    else if (_strnicmp(exe_path, "\\SystemRoot\\", strlen("\\SystemRoot\\")) == 0) {
        char* system_root_dup = NULL;
        size_t len = 0;
        errno_t err;

        // Attempt to get "SystemRoot"
        err = _dupenv_s(&system_root_dup, &len, "SystemRoot");
        if (err || system_root_dup == NULL) {
            // Attempt to get "SYSTEMROOT" if "SystemRoot" fails
            err = _dupenv_s(&system_root_dup, &len, "SYSTEMROOT");
            if (err || system_root_dup == NULL) {
                // Unable to retrieve "SystemRoot" or "SYSTEMROOT"
                add_assoc_string(return_array, "exe", exe_path, 1);
            }
        }

        if (system_root_dup) {
            char* formatted_path = malloc(strlen(system_root_dup) + strlen(exe_path + strlen("\\SystemRoot")) + 1);
            if (formatted_path) {
                sprintf(formatted_path, "%s%s", system_root_dup, exe_path + strlen("\\SystemRoot"));
                add_assoc_string(return_array, "exe", formatted_path, 1);
                free(formatted_path);
            }
            free(system_root_dup);
        }
        else {
            add_assoc_string(return_array, "exe", exe_path, 1);
        }
    }
    else {
        add_assoc_string(return_array, "exe", exe_path, 1);
    }

    /* Memory information */
    array_init(&mem);
    add_assoc_zval(return_array, "mem", &mem);
    add_assoc_long(&mem, "page_fault_count", (long)memory.PageFaultCount);
    add_assoc_long(&mem, "peak_working_set_size", (long)memory.PeakWorkingSetSize);
    add_assoc_long(&mem, "working_set_size", (long)memory.WorkingSetSize);
    add_assoc_long(&mem, "quota_peak_paged_pool_usage", (long)memory.QuotaPeakPagedPoolUsage);
    add_assoc_long(&mem, "quota_paged_pool_usage", (long)memory.QuotaPagedPoolUsage);
    add_assoc_long(&mem, "quota_peak_non_paged_pool_usage", (long)memory.QuotaPeakNonPagedPoolUsage);
    add_assoc_long(&mem, "quota_non_paged_pool_usage", (long)memory.QuotaNonPagedPoolUsage);
    add_assoc_long(&mem, "pagefile_usage", (long)memory.PagefileUsage);
    add_assoc_long(&mem, "peak_pagefile_usage", (long)memory.PeakPagefileUsage);

    /* Time information */
    array_init(&tms);
    add_assoc_zval(return_array, "tms", &tms);

    /* Current system time */
    SYSTEMTIME now_sys;
    FILETIME now_file;
    ULONGLONG nowx;

    GetSystemTime(&now_sys);
    SystemTimeToFileTime(&now_sys, &now_file);
    memcpy(&nowx, &now_file, sizeof(nowx));

    /* Creation time */
    memcpy(&creatx, &creat, sizeof(creatx));
    /* Kernel time */
    memcpy(&kernx, &kern, sizeof(kernx));
    /* User time */
    memcpy(&userx, &user, sizeof(userx));

    add_assoc_double(&tms, "created", (double)((nowx - creatx) / 10000ULL) / 1000.0);
    add_assoc_double(&tms, "kernel", (double)(kernx / 10000ULL) / 1000.0);
    add_assoc_double(&tms, "user", (double)(userx / 10000ULL) / 1000.0);

    return SUCCESS;
}
/* }}} */

/* {{{ php_win32ps_meminfo */
PHP_WIN32PS_API void php_win32ps_meminfo(zval* return_array)
{
    MEMORYSTATUSEX memory = { 0 };
    memory.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memory);

    add_assoc_long(return_array, "load", (long)memory.dwMemoryLoad);
    add_assoc_long(return_array, "unit", 1024L);
    add_assoc_long(return_array, "total_phys", (long)(memory.ullTotalPhys / 1024ULL));
    add_assoc_long(return_array, "avail_phys", (long)(memory.ullAvailPhys / 1024ULL));
    add_assoc_long(return_array, "total_pagefile", (long)(memory.ullTotalPageFile / 1024ULL));
    add_assoc_long(return_array, "avail_pagefile", (long)(memory.ullAvailPageFile / 1024ULL));
    add_assoc_long(return_array, "total_virtual", (long)(memory.ullTotalVirtual / 1024ULL));
    add_assoc_long(return_array, "avail_virtual", (long)(memory.ullAvailVirtual / 1024ULL));
}
/* }}} */

/* {{{ proto array win32_ps_list_procs()
    List running processes */
PHP_FUNCTION(win32_ps_list_procs)
{
    DWORD processes[PHP_WIN32PS_MAXPROC];
    DWORD proc_count = 0;

    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    if (!EnumProcesses(processes, sizeof(processes), &proc_count)) {
        php_win32ps_error("Could not enumerate running processes", 0);
        RETURN_FALSE;
    }

    array_init(return_value);
    for (DWORD i = 0; i < proc_count / sizeof(DWORD); ++i) {
        zval entry;
        array_init(&entry);

        if (php_win32ps_procinfo(processes[i], &entry, PHP_WIN32PS_HANDLE_OPS) == SUCCESS) {
            add_next_index_zval(return_value, &entry);
        }
        else {
            zval_ptr_dtor(&entry);
        }
    }
}
/* }}} */

/* {{{ proto array win32_ps_stat_proc([int pid])
    Get process info of process with pid or the current process */
PHP_FUNCTION(win32_ps_stat_proc)
{
    zend_long process = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(process)
        ZEND_PARSE_PARAMETERS_END();

    if (process <= 0) {
        process = GetCurrentProcessId();
    }

    zval proc_info;
    if (php_win32ps_procinfo((DWORD)process, &proc_info, PHP_WIN32PS_ALL) == FAILURE) {
        RETURN_FALSE;
    }

    RETURN_ZVAL(&proc_info, 1, 0);
}
/* }}} */

/* {{{ proto array win32_ps_stat_mem()
    Get memory info */
PHP_FUNCTION(win32_ps_stat_mem)
{
    if (zend_parse_parameters_none() == FAILURE) {
        RETURN_FALSE;
    }

    zval mem_info;
    php_win32ps_meminfo(&mem_info);
    RETURN_ZVAL(&mem_info, 1, 0);
}
/* }}} */
