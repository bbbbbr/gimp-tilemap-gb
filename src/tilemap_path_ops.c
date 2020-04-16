//
// tilemap_path_ops.c
//

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "tilemap_path_ops.h"


const char kPathSeparator =
#ifdef _WIN32
                            '\\';
#else
                            '/';
#endif


const char * get_filename_from_path(const char * path)
{
    size_t i;

   for(i = strlen(path) - 1; i; i--) {
        if (path[i] == kPathSeparator) {
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
        if (path[i] == kPathSeparator) {

            memcpy(path_only, path, i+1 );
            path_only[i+1] = '\0';
            return true;
        }
    }

    memcpy(path_only, path, strlen(path));
    return true;
}
