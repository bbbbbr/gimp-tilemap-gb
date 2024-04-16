//
// tilemap_path_ops.c
//

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "logging.h"

#include "tilemap_path_ops.h"


const char kExtensionSeparator = '.';
const char kPathSeparator =

#ifdef _WIN32
  #ifndef _WIN32
     #define __WIN32__
  #endif
#endif

#ifdef __WIN32__
                            '\\';
#else
                            '/';
#endif

const char kPathSeparator_unix = '/';

const char * get_filename_from_path(const char * path)
{
    size_t i;

    // Returns string starting at last occurrance of path separator char
    for(i = strlen(path) - 1; i; i--) {

    // Add trailing slash to path if needed (Windows needs both for when running under linix like env)
    #ifdef __WIN32__
        if ((path[i] == kPathSeparator) || (path[i] == kPathSeparator_unix))
    #else
        if (path[i] == kPathSeparator)
    #endif
        {
            return &path[i+1];
        }
    }
    return path;
}

int32_t get_path_without_filename(const char * path, char * path_only, uint32_t str_max)
{
    size_t i;

   if (strlen(path) + 1 > str_max)
        return false;

   for(i = strlen(path) - 1; i; i--) {

    // Add trailing slash to path if needed (Windows needs both for when running under linix like env)
    #ifdef __WIN32__
        if ((path[i] == kPathSeparator) || (path[i] == kPathSeparator_unix))
    #else
        if (path[i] == kPathSeparator)
    #endif
        {
            memcpy(path_only, path, i+1 );
            path_only[i+1] = '\0';
            return true;
        }
    }
    // memcpy(path_only, path, strlen(path));
    // No separater found, so no path
    path_only[0] = '\0';
    return true;
}


void copy_filename_without_path_and_extension(char * path_out, const char * path_in)
{
    size_t i;
    char * last_ext;

   // First make sure any path seperators are removed
   for(i = strlen(path_in) - 1; i; i--) {
        if (path_in[i] == kPathSeparator) {
            path_in = &path_in[i+1];
            // log_verbose("Found: %s\n", path_in);
            break;
        }
    }

    memcpy(path_out, path_in, strlen(path_in));

    // Then check to see if there is an extension (*FIRST* occurance of '.')
    // (tries to remove *all* trailing extensions)
    last_ext = strrchr (path_out, kExtensionSeparator);
    while (last_ext) {

        if (last_ext != NULL) {
            // If an extension is found then overwrite it with a string terminator
            *last_ext = '\0';
            // log_verbose("Truncating extension: %s\n", path_out);
        }

        last_ext = strrchr (path_out, kExtensionSeparator);
    }
}


void copy_filename_without_extension(char * path_out, const char * path_in)
{
    char * last_ext;
    char * last_slash;

    // Copy string to start with
    strncpy(path_out, path_in, STR_FILENAME_MAX);

    // Find the last path separator if present
    // Starting from here ensures that no path ".." characters
    // get mistaken as extension delimiters.
    last_slash = strrchr (path_out, kExtensionSeparator);
    if (!last_slash)
        last_slash = path_out;

    // Then check to see if there is an extension (*FIRST* occurance of '.')
    // (tries to remove *all* trailing extensions)
    last_ext = strrchr (last_slash, kExtensionSeparator);
    while (last_ext) {

        if (last_ext != NULL) {
            // If an extension is found then overwrite it with a string terminator
            *last_ext = '\0';
            // log_verbose("Truncating extension: %s\n", last_slash);
        }

        last_ext = strrchr (last_slash, kExtensionSeparator);
    }
}


// Case insensitive
bool matches_extension(char * filename, char * extension) {

    if (strlen(filename) >= strlen(extension)) {
        char * str_ext = filename + (strlen(filename) - strlen(extension));

        return (strncasecmp(str_ext, extension, strlen(extension)) == 0);
    }
    else
        return false;
}
