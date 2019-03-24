//
// lib_gbr_file_utils.c
//

#include "lib_gbr_file_utils.h"

int32_t gbr_read_header_key(FILE * p_file) {

    // Check to make sure the start of the file is "GBO"
    int8_t sig[3];

    if (fread(sig, 3, 1, p_file))
        if ((sig[0] == 'G') && (sig[1] == 'B') && (sig[2] == 'O'))
            return true; // return success

    // Return failed - didn't find header
    return false;
 }


int32_t gbr_read_version(FILE * p_file) {

    // Check to make sure the start of the file is "GBO"
    uint8_t version;

    if (fread(&version, 1, 1, p_file))
        return true; // return success

    // Return failed - didn't read version
    return false;
 }



int32_t gbr_read_object_from_file(pascal_file_object * p_obj, FILE * p_file) {

    // Read in the object properties, and then it's data buffer
    if (fread(&(p_obj->type),         sizeof(p_obj->type),         1, p_file))
      if (fread(&(p_obj->id),           sizeof(p_obj->id),           1, p_file))
        if (fread(&(p_obj->length_bytes), sizeof(p_obj->length_bytes), 1, p_file)
            && (p_obj->length_bytes <= PASCAL_OBJECT_MAX_SIZE) )
          if (fread(p_obj->p_data,          p_obj->length_bytes,         1, p_file)) {
//            printf(" TRUE type=%x, id=%x, size=%x\n", p_obj->type, p_obj->id, p_obj->length_bytes);
            p_obj->offset = 0;
            return true;
          }

// printf(" FALSE type=%x, id=%x, size=%x\n", p_obj->type, p_obj->id, p_obj->length_bytes);

    // If all the above reads didn't complete then signal failure
    return false;
 }



// WARNING: Expects buffer size to be (n_bytes + 1) for appending null terminator
void gbr_read_str(int8_t * p_dest_str, pascal_file_object * p_obj, uint32_t n_bytes) {

    // Copy string, add terminator, move offset to next data
    memcpy(p_dest_str, &p_obj->p_data[ p_obj->offset ], n_bytes);
    p_dest_str[n_bytes + 1] = '\0';
    p_obj->offset += n_bytes;

    printf("gbr_read_str @ %d\n", p_obj->offset);
}

void gbr_read_buf(int8_t * p_dest_buf, pascal_file_object * p_obj, uint32_t n_bytes) {

    printf("gbr_read_buf @ %d -> ", p_obj->offset);

    // Copy string, add terminator, move offset to next data
    memcpy(p_dest_buf, &(p_obj->p_data[ p_obj->offset ]), n_bytes);
    p_obj->offset += n_bytes;

    printf("%d\n", p_obj->offset);
}

void gbr_read_uint32(uint32_t * p_dest_val, pascal_file_object * p_obj) {

    memcpy(p_dest_val, &p_obj->p_data[ p_obj->offset ], sizeof(uint32_t));
    p_obj->offset += sizeof(uint32_t);

    printf("gbr_read_uint32 @ %d\n", p_obj->offset);
}

void gbr_read_uint16(uint16_t * p_dest_val, pascal_file_object * p_obj) {

    memcpy(p_dest_val, &p_obj->p_data[ p_obj->offset ], sizeof(uint16_t));
    p_obj->offset += sizeof(uint16_t);

    printf("gbr_read_uint16 @ %d\n", p_obj->offset);
}

void gbr_read_uint8(uint8_t * p_dest_val, pascal_file_object * p_obj) {

    *p_dest_val = p_obj->p_data[ p_obj->offset ];
    p_obj->offset += sizeof(uint8_t);

    printf("gbr_read_uint8 @ %d\n", p_obj->offset);
}


void gbr_read_bool(uint8_t * p_dest_val, pascal_file_object * p_obj) {

    *p_dest_val = p_obj->p_data[ p_obj->offset ] & 0x01;
    p_obj->offset += sizeof(uint8_t);

    printf("gbr_read_bool @ %d\n", p_obj->offset);
}
