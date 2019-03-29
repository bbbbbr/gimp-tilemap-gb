//
// lib_gbm_file_utils.h
//

#include "lib_gbm.h"

#ifndef LIB_GBM_FILE_UTILS_FILE_HEADER
#define LIB_GBM_FILE_UTILS_FILE_HEADER


int32_t gbm_read_header_key(FILE * p_file);
int32_t gbm_read_version(FILE * p_file);
int32_t gbm_write_header_key(FILE * p_file);
int32_t gbm_write_version(FILE * p_file);
int32_t gbm_read_object_from_file(gbm_file_object * g_obj, FILE * p_file);
int32_t gbm_write_object_to_file(gbm_file_object * g_obj, FILE * p_file);


#endif