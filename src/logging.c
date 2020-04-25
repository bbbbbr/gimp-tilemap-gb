// logging.c

#include <stdarg.h>
#include <stdio.h>

#include "logging.h"

int output_level = OUTPUT_LEVEL_DEFAULT;


void log_set_level(int new_output_level) {
    output_level = new_output_level;
}

void log_verbose(const char * format, ...){

    va_list args;

    // Only print if vermose mode IS enabled
    if (output_level != OUTPUT_LEVEL_VERBOSE) return;

    va_start (args, format);
    vprintf (format, args);
    va_end (args);
}

void log_standard(const char * format, ...){

    va_list args;

    // Only print if quiet mode and error_only are NOT enabled
    if ((output_level == OUTPUT_LEVEL_QUIET) ||
        (output_level == OUTPUT_LEVEL_ONLY_ERRORS)) return;

    va_start (args, format);
    vprintf (format, args);
    va_end (args);
}

void log_error(const char * format, ...){

    va_list args;

    // Only print if quiet mode is NOT enabled
    if (output_level == OUTPUT_LEVEL_QUIET) return;

    va_start (args, format);
    vprintf (format, args);
    va_end (args);
}