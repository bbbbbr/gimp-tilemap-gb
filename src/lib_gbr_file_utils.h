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

int32_t gbr_read_object_from_file(gbr_file_object * p_obj, FILE * p_file);
int32_t gbr_write_object_to_file(gbr_file_object * p_obj, FILE * p_file);

void gbr_read_str(char * p_dest_str, gbr_file_object * p_obj, uint32_t n_bytes);
void gbr_read_padding_bytes(gbr_file_object * p_obj, uint32_t n_bytes);
void gbr_read_buf(uint8_t * p_dest_buf, gbr_file_object * p_obj, uint32_t n_bytes);
void gbr_read_uint32(uint32_t * p_dest_val, gbr_file_object * p_obj);
void gbr_read_uint16(uint16_t * p_dest_val, gbr_file_object * p_obj);
void gbr_read_uint8(uint8_t * p_dest_val, gbr_file_object * p_obj);
void gbr_read_bool(uint8_t * p_dest_val, gbr_file_object * p_obj);

void gbr_write_padding(gbr_file_object * p_obj, uint32_t n_bytes);
void gbr_write_str(char * p_src_str, gbr_file_object * p_obj, uint32_t n_bytes);
void gbr_write_buf(uint8_t * p_src_buf, gbr_file_object * p_obj, uint32_t n_bytes);
void gbr_write_uint32(uint32_t * p_src_val, gbr_file_object * p_obj);
void gbr_write_uint16(uint16_t * p_src_val, gbr_file_object * p_obj);
void gbr_write_uint8(uint8_t * p_src_val, gbr_file_object * p_obj);
void gbr_write_bool(uint8_t * p_src_val, gbr_file_object * p_obj);



#endif