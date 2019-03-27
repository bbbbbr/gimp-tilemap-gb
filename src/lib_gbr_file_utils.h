//
// lib_gbr_file_utils.h
//

#include "lib_gbr.h"

#ifndef LIB_GBR_FILE_UTILS_FILE_HEADER
#define LIB_GBR_FILE_UTILS_FILE_HEADER


int32_t gbr_read_header_key(FILE * p_file);
int32_t gbr_read_version(FILE * p_file);
int32_t gbr_write_header_key(FILE * p_file);
int32_t gbr_write_version(FILE * p_file);

int32_t gbr_read_object_from_file(pascal_file_object * p_obj, FILE * p_file);
int32_t gbr_write_object_to_file(pascal_file_object * p_obj, FILE * p_file);

void gbr_read_str(int8_t * p_dest_str, pascal_file_object * p_obj, uint32_t n_bytes);
void gbr_read_buf(int8_t * p_dest_buf, pascal_file_object * p_obj, uint32_t n_bytes);
void gbr_read_uint32(uint32_t * p_dest_val, pascal_file_object * p_obj);
void gbr_read_uint16(uint16_t * p_dest_val, pascal_file_object * p_obj);
void gbr_read_uint8(uint8_t * p_dest_val, pascal_file_object * p_obj);
void gbr_read_bool(uint8_t * p_dest_val, pascal_file_object * p_obj);

void gbr_write_padding(pascal_file_object * p_obj, uint32_t n_bytes);
void gbr_write_str(int8_t * p_src_str, pascal_file_object * p_obj, uint32_t n_bytes);
void gbr_write_buf(int8_t * p_src_buf, pascal_file_object * p_obj, uint32_t n_bytes);
void gbr_write_uint32(uint32_t * p_src_val, pascal_file_object * p_obj);
void gbr_write_uint16(uint16_t * p_src_val, pascal_file_object * p_obj);
void gbr_write_uint8(uint8_t * p_src_val, pascal_file_object * p_obj);
void gbr_write_bool(uint8_t * p_src_val, pascal_file_object * p_obj);



#endif