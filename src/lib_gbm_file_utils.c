//
// lib_gbm_file_utils.c
//

#include "lib_gbm_file_utils.h"


char gbm_object_marker[0x6] = {"HPJMTL"};// {0x48, 0x50, 0x4A, 0x4D, 0x54, 0x4C }; /* "HPJMTL" */


int32_t gbm_read_header_key(FILE * p_file) {

    // Check to make sure the start of the file is "GBO"
    char sig[3];

    if (fread(sig, 3, 1, p_file))
        if ((sig[0] == 'G') && (sig[1] == 'B') && (sig[2] == 'O'))
            return true; // return success

    // Return failed - didn't find header
    return false;
 }


int32_t gbm_read_version(FILE * p_file) {

    // Check to make sure the version is "0"
    uint8_t version;

    if (fread(&version, 1, 1, p_file))
        if (version == '1')
            return true; // return success

    // Return failed - didn't read version
    return false;
 }


int32_t gbm_write_header_key(FILE * p_file) {

    // Check to make sure the start of the file is "GBO"

    char sig[3] = {'G', 'B', 'O'};

    if (fwrite(sig, 3, 1, p_file))
        return true; // return success
    else
        return false; // // Return failed - didn't write header
 }


int32_t gbm_write_version(FILE * p_file) {

    // Write default version to GBR file
    char version[3] = {'1'};

    if (fwrite(version, 1, 1, p_file))
        return true; // return success

    // Return failed - didn't write version
    return false;
 }



int32_t gbm_read_object_from_file(gbm_file_object * p_obj, FILE * p_file) {

// printf("ftell: %ld\n", ftell(p_file));

    // Read in the object properties, and then it's data buffer
    if ((fread(p_obj->marker,                   sizeof(p_obj->marker),       1, p_file))
        && (0 == strncmp(p_obj->marker, gbm_object_marker, sizeof(p_obj->marker)))) {
        if (fread(&(p_obj->id),                   sizeof(p_obj->id),            1, p_file)) {
            if (fread(&(p_obj->object_id),         sizeof(p_obj->object_id),      1, p_file)) {
                if (fread(&(p_obj->master_id),      sizeof(p_obj->master_id),       1, p_file)) {
                    if (fread(&(p_obj->crc),         sizeof(p_obj->crc),             1, p_file)) {
                        if (fread(&(p_obj->length_bytes), sizeof(p_obj->length_bytes), 1, p_file)
                            && (p_obj->length_bytes <= GBM_OBJECT_MAX_SIZE) )
                        {
                            if (p_obj->length_bytes == 0) {
//                                printf("GBM TRUE ZERO OBJ type=%x, id=%x, size=%x\n", p_obj->id, p_obj->object_id, p_obj->length_bytes);
                                p_obj->offset = 0;
                                return true;
                            }
                            else if (fread(p_obj->p_data,          p_obj->length_bytes,         1, p_file)) {
//                                printf("GBM TRUE type=%x, id=%x, size=%x\n", p_obj->id, p_obj->object_id, p_obj->length_bytes);
                                p_obj->offset = 0;
                                return true;
                            }
                        }
                    }
                }
            }
        }
    } // All these parenthesis to satisfy "-Wparentheses" during cross compile
    printf("ftell: %ld\n", ftell(p_file));

    printf("GBM FALSE type=%x, id=%x, size=%x\n", p_obj->id, p_obj->object_id, p_obj->length_bytes);

    // If all the above reads didn't complete then signal failure
    return false;
 }



int32_t gbm_write_object_to_file(gbm_file_object * p_obj, FILE * p_file) {

    p_obj-> crc = 0x00; // Always zero

    // Read in the object properties, and then it's data buffer
    if (fwrite(&(gbm_object_marker[0]),           sizeof(p_obj->marker),       1, p_file))
        if (fwrite(&(p_obj->id),                   sizeof(p_obj->id),            1, p_file))
            if (fwrite(&(p_obj->object_id),         sizeof(p_obj->object_id),      1, p_file))
                if (fwrite(&(p_obj->master_id),      sizeof(p_obj->master_id),       1, p_file))
                    if (fwrite(&(p_obj->crc),         sizeof(p_obj->crc),             1, p_file))
                        if (fwrite(&(p_obj->length_bytes), sizeof(p_obj->length_bytes), 1, p_file)
                            && (p_obj->length_bytes <= GBM_OBJECT_MAX_SIZE) )
                            if ((fwrite(p_obj->p_data, p_obj->length_bytes, 1, p_file))
                                || (p_obj->length_bytes == 0)) {
//                                printf("GBM WROTE type=%x, id=%x, size=%x\n", p_obj->id, p_obj->object_id, p_obj->length_bytes);
                                return true;
                            }

    printf("GBM FALSE id=%x, object_id=%x, size=%x\n", p_obj->id, p_obj->object_id, p_obj->length_bytes);

    // If all the above reads didn't complete then signal failure
    return false;
 }



// =========  WRITE/EXPORT UTILITY FUNCTIONS =========


// WARNING: Expects buffer size to be (n_bytes + 1) for appending null terminator
void gbm_read_str(char * p_dest_str, gbm_file_object * p_obj, uint32_t n_bytes) {

    // Copy string, add terminator, move offset to next data
    memcpy(p_dest_str, &p_obj->p_data[ p_obj->offset ], n_bytes);
    p_dest_str[n_bytes + 1] = '\0';
    p_obj->offset += n_bytes;

    printf("gbm_read_str @ %d\n", p_obj->offset);
}


void gbm_read_buf(uint8_t * p_dest_buf, gbm_file_object * p_obj, uint32_t n_bytes) {

    printf("gbm_read_buf @ %d -> ", p_obj->offset);

    // Copy data, move offset to next data
    memcpy(p_dest_buf, &(p_obj->p_data[ p_obj->offset ]), n_bytes);
    p_obj->offset += n_bytes;

    printf("%d\n", p_obj->offset);
}


void gbm_read_uint32(uint32_t * p_dest_val, gbm_file_object * p_obj) {

    memcpy(p_dest_val, &p_obj->p_data[ p_obj->offset ], sizeof(uint32_t));
    p_obj->offset += sizeof(uint32_t);

    printf("gbm_read_uint32 @ %d=%d\n", p_obj->offset, *p_dest_val);
}


void gbm_read_uint16(uint16_t * p_dest_val, gbm_file_object * p_obj) {

    memcpy(p_dest_val, &p_obj->p_data[ p_obj->offset ], sizeof(uint16_t));
    p_obj->offset += sizeof(uint16_t);

    printf("gbm_read_uint16 @ %d=%d\n", p_obj->offset, *p_dest_val);
}


void gbm_read_uint8(uint8_t * p_dest_val, gbm_file_object * p_obj) {

    *p_dest_val = p_obj->p_data[ p_obj->offset ];
    p_obj->offset += sizeof(uint8_t);

    printf("gbm_read_uint8 @ %d=%d\n", p_obj->offset, *p_dest_val);
}


void gbm_read_bool(uint8_t * p_dest_val, gbm_file_object * p_obj) {

    *p_dest_val = p_obj->p_data[ p_obj->offset ] & 0x01;
    p_obj->offset += sizeof(uint8_t);

    printf("gbm_read_bool @ %d=%d\n", p_obj->offset, *p_dest_val);
}


// =========  WRITE/EXPORT UTILITY FUNCTIONS =========


void gbm_write_padding(gbm_file_object * p_obj, uint32_t n_bytes) {

    printf("gbm_write_padding of %d @ %d -> ", n_bytes, p_obj->offset);

    memset(&p_obj->p_data[ p_obj->offset ], 0x00, n_bytes);
    p_obj->offset += n_bytes;
    p_obj->length_bytes = p_obj->offset;

    printf("%d\n", p_obj->offset);

    printf(" PAD id=%x, object_id=%x, size=%x\n, offset=%x", p_obj->id,
                                                        p_obj->object_id,
                                                        p_obj->length_bytes,
                                                        p_obj->offset);
}


// WARNING: Expects source buffer size to be (n_bytes + 1) for appending null terminator
void gbm_write_str(char * p_src_str, gbm_file_object * p_obj, uint32_t n_bytes) {

    // Copy string (without terminator), move offset to next data
    memcpy(&p_obj->p_data[ p_obj->offset ], p_src_str, n_bytes);
    p_obj->offset += n_bytes;
    p_obj->length_bytes = p_obj->offset;

    printf("gbm_write_str @ %d (nbytes=%d)\n", p_obj->offset, n_bytes);
}


void gbm_write_buf(uint8_t * p_src_buf, gbm_file_object * p_obj, uint32_t n_bytes) {

    printf("gbm_write_buf @ %d -> ", p_obj->offset);

    // Copy data, move offset to next data
    memcpy(&(p_obj->p_data[ p_obj->offset ]), p_src_buf, n_bytes);
    p_obj->offset += n_bytes;
    p_obj->length_bytes = p_obj->offset;

    printf("%d\n", p_obj->offset);
}


void gbm_write_uint32(uint32_t * p_src_val, gbm_file_object * p_obj) {

    memcpy(&p_obj->p_data[ p_obj->offset ], p_src_val, sizeof(uint32_t));
    p_obj->offset += sizeof(uint32_t);
    p_obj->length_bytes = p_obj->offset;

    printf("gbm_write_uint32 @ %d=%d\n", p_obj->offset, *p_src_val);
}


void gbm_write_uint16(uint16_t * p_src_val, gbm_file_object * p_obj) {

    memcpy(&p_obj->p_data[ p_obj->offset ], p_src_val, sizeof(uint16_t));
    p_obj->offset += sizeof(uint16_t);
    p_obj->length_bytes = p_obj->offset;

    printf("gbm_write_uint16 @ %d=%d\n", p_obj->offset, *p_src_val);
}


void gbm_write_uint8(uint8_t * p_src_val, gbm_file_object * p_obj) {

    p_obj->p_data[ p_obj->offset ] = *p_src_val;
    p_obj->offset += sizeof(uint8_t);
    p_obj->length_bytes = p_obj->offset;

    printf("gbm_write_uint8 @ %d=%d\n", p_obj->offset, *p_src_val);
}


void gbm_write_bool(uint8_t * p_src_val, gbm_file_object * p_obj) {

    p_obj->p_data[ p_obj->offset ] = *p_src_val & 0x01;
    p_obj->offset += sizeof(uint8_t);
    p_obj->length_bytes = p_obj->offset;

    printf("gbm_write_bool @ %d=%d\n", p_obj->offset, *p_src_val);
}


