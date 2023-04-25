#ifndef WASMVM_EXEC_COLOR_DEF
#define WASMVM_EXEC_COLOR_DEF

#ifdef CONSOLE_COLOR
    #define COLOR_Warning "\033[1;33mwarning\033[0m"
    #define COLOR_Error "\033[1;31merror\033[0m"
    #define COLOR_Note "\033[1;36mnote\033[0m"
#else
    #define COLOR_Warning "warning"
    #define COLOR_Error "error"
    #define COLOR_Note "note"
#endif

#endif