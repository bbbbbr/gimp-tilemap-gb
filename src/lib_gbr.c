//
// lib_gbr.h
//

#include "lib_gbr.h"
#include "lib_gbr_import.h"
#include "lib_gbr_ops.h"
#include "lib_gbr_file_utils.h"



int32_t gbr_load(const int8_t * filename) {

    FILE               * p_file;
    pascal_file_object   obj;
    gbr_record           gbr;

    obj.p_data = malloc(PASCAL_OBJECT_MAX_SIZE);

    // open the file
    p_file = fopen(filename, "rb");
    if (p_file == NULL)
        return false;

    if (gbr_read_header_key(p_file)) {

        printf("Found Header\n");

        if (gbr_read_version(p_file)) {

            // Read objects from the file until it's finished
            while (gbr_read_object_from_file(&obj, p_file)) {

                printf("OBJ: type=%d, id=%d, size=%d\n", obj.type, obj.id, obj.length_bytes);
                printf("OBJ: type=%02x, id=%02x, size=%04x\n", obj.type, obj.id, obj.length_bytes);

                switch (obj.type) {
                    // Process Object
                    case gbr_obj_producer: printf("gbr_producer\n");
                                       gbr_object_producer_decode(&gbr, &obj);
                                       break;

                    case gbr_obj_tile_data: printf("gbr_tile_data\n");
                                        gbr_object_tile_data_decode(&gbr, &obj);
                                        break;

                    case gbr_obj_tile_settings: printf("gbr_tile_settings\n");
                                            gbr_object_tile_settings_decode(&gbr, &obj);
                                            break;

                    case gbr_obj_tile_export: printf("gbr_tile_export\n");
                                          gbr_object_tile_export_decode(&gbr, &obj);
                                          break;

                    case gbr_obj_tile_import: printf("gbr_tile_import\n");
                                          gbr_object_tile_import_decode(&gbr, &obj);
                                          break;

                    case gbr_obj_palettes: printf("gbr_palettes\n");
                                       gbr_object_palettes_decode(&gbr, &obj);
                                       break;

                    case gbr_obj_tile_pal: printf("gbr_tile_pal\n");
                                       gbr_object_tile_pal_decode(&gbr, &obj);
                                       break;

                    case gbr_obj_deleted: printf("gbr_deleted\n");
                                      break;
                }
            }
        }
    }

    fclose(p_file);
}


int main()
{
   printf("Loading\n");

   gbr_load("test.gbr");

   return 0;
}


