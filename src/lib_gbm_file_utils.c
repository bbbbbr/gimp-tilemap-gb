//
// lib_gbm_file_utils.c
//

#include "lib_gbm_file_utils.h"


int8_t gbm_object_marker[0x6] = {0x48, 0x50, 0x4A, 0x4D, 0x54, 0x4C }; /* "HPJMTL" */


int32_t gbm_read_header_key(FILE * p_file) {

    // Check to make sure the start of the file is "GBO"
    int8_t sig[3];

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

    int8_t sig[3] = {'G', 'B', 'O'};

    if (fwrite(sig, 3, 1, p_file))
        return true; // return success
    else
        return false; // // Return failed - didn't write header
 }


int32_t gbm_write_version(FILE * p_file) {

    // Write default version to GBR file
    int8_t version[3] = {'1'};

    if (fwrite(version, 1, 1, p_file))
        return true; // return success

    // Return failed - didn't write version
    return false;
 }



int32_t gbm_read_object_from_file(gbm_file_object * g_obj, FILE * p_file) {

// printf("ftell: %ld\n", ftell(p_file));

    // Read in the object properties, and then it's data buffer
    if ((fread(g_obj->marker,                   sizeof(g_obj->marker),       1, p_file))
        && (0 == strncmp(g_obj->marker, gbm_object_marker, sizeof(g_obj->marker))))
        if (fread(&(g_obj->id),                   sizeof(g_obj->id),            1, p_file))
            if (fread(&(g_obj->object_id),         sizeof(g_obj->object_id),      1, p_file))
                if (fread(&(g_obj->master_id),      sizeof(g_obj->master_id),       1, p_file))
                    if (fread(&(g_obj->crc),         sizeof(g_obj->crc),             1, p_file))
                        if (fread(&(g_obj->length_bytes), sizeof(g_obj->length_bytes), 1, p_file)
                            && (g_obj->length_bytes <= PASCAL_OBJECT_MAX_SIZE) )
                            if (g_obj->length_bytes == 0) {
//                                printf("GBM TRUE ZERO OBJ type=%x, id=%x, size=%x\n", g_obj->id, g_obj->object_id, g_obj->length_bytes);
                                g_obj->offset = 0;
                                return true;
                            }
                            else if (fread(g_obj->p_data,          g_obj->length_bytes,         1, p_file)) {
//                                printf("GBM TRUE type=%x, id=%x, size=%x\n", g_obj->id, g_obj->object_id, g_obj->length_bytes);
                                g_obj->offset = 0;
                                return true;
                            }
    printf("ftell: %ld\n", ftell(p_file));

    printf("GBM FALSE type=%x, id=%x, size=%x\n", g_obj->id, g_obj->object_id, g_obj->length_bytes);

    // If all the above reads didn't complete then signal failure
    return false;
 }



int32_t gbm_write_object_from_file(gbm_file_object * g_obj, FILE * p_file) {

    // Read in the object properties, and then it's data buffer
    if (fwrite(&(gbm_object_marker[0]),           sizeof(g_obj->marker),       1, p_file))
        if (fwrite(&(g_obj->id),                   sizeof(g_obj->id),            1, p_file))
            if (fwrite(&(g_obj->object_id),         sizeof(g_obj->object_id),      1, p_file))
                if (fwrite(&(g_obj->master_id),      sizeof(g_obj->master_id),       1, p_file))
                    if (fwrite(&(g_obj->crc),         sizeof(g_obj->crc),             1, p_file))
                        if (fwrite(&(g_obj->length_bytes), sizeof(g_obj->length_bytes), 1, p_file)
                            && (g_obj->length_bytes <= PASCAL_OBJECT_MAX_SIZE) )
                            if (g_obj->length_bytes == 0) {
//                                printf("GBM TRUE ZERO OBJ type=%x, id=%x, size=%x\n", g_obj->id, g_obj->object_id, g_obj->length_bytes);
                                g_obj->offset = 0;
                                return true;
                            }
                            else if (fwrite(g_obj->p_data,          g_obj->length_bytes,         1, p_file)) {
//                                printf("GBM WROTE type=%x, id=%x, size=%x\n", g_obj->id, g_obj->object_id, g_obj->length_bytes);
                                g_obj->offset = 0;
                                return true;
                            }

    printf("GBM FALSE type=%x, id=%x, size=%x\n", g_obj->id, g_obj->object_id, g_obj->length_bytes);

    // If all the above reads didn't complete then signal failure
    return false;
 }


