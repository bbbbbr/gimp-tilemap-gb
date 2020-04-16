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

void gbm_read_str(char * p_dest_str, gbm_file_object * p_obj, uint32_t n_bytes);
void gbm_read_buf(uint8_t * p_dest_buf, gbm_file_object * p_obj, uint32_t n_bytes);
void gbm_read_uint32(uint32_t * p_dest_val, gbm_file_object * p_obj);
void gbm_read_uint16(uint16_t * p_dest_val, gbm_file_object * p_obj);
void gbm_read_uint8(uint8_t * p_dest_val, gbm_file_object * p_obj);
void gbm_read_bool(uint8_t * p_dest_val, gbm_file_object * p_obj);

void gbm_write_padding(gbm_file_object * p_obj, uint32_t n_bytes);
void gbm_write_str(char * p_src_str, gbm_file_object * p_obj, uint32_t n_bytes);
void gbm_write_buf(uint8_t * p_src_buf, gbm_file_object * p_obj, uint32_t n_bytes);
void gbm_write_uint32(uint32_t * p_src_val, gbm_file_object * p_obj);
void gbm_write_uint16(uint16_t * p_src_val, gbm_file_object * p_obj);
void gbm_write_uint8(uint8_t * p_src_val, gbm_file_object * p_obj);
void gbm_write_bool(uint8_t * p_src_val, gbm_file_object * p_obj);

#endif