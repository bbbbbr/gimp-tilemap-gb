//
// lib_gbr_import.c
//

#include "lib_gbr_import.h"
#include "lib_gbr_file_utils.h"
#include "lib_gbr_ops.h"


int32_t gbr_object_producer_decode(gbr_record * p_gbr, gbr_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_PRODUCER_SIZE)
        return false;

    gbr_read_str(p_gbr->producer.name,    p_obj, GBR_PRODUCER_NAME_SIZE);
    gbr_read_str(p_gbr->producer.version, p_obj, GBR_PRODUCER_VERSION_SIZE);
    gbr_read_str(p_gbr->producer.info,    p_obj, GBR_PRODUCER_INFO_SIZE);

    printf("PRODUCER:\n%s\n%s\n%s\n", p_gbr->producer.name,
                                      p_gbr->producer.version,
                                      p_gbr->producer.info);

    return true;
}



// Tile: x,y,id = .tile_list[ .height
//                            + (y * .width)
//                            + (id * width * height) ]
// TODO: change to int16_t? read about what is most efficient on 64 bit architectures
int32_t gbr_object_tile_data_decode(gbr_record * p_gbr, gbr_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_TILE_DATA_SIZE_MIN)
        return false;

    gbr_read_str   ( p_gbr->tile_data.name,      p_obj, GBR_TILE_DATA_NAME_SIZE);
    gbr_read_uint16(&p_gbr->tile_data.width,     p_obj);
    gbr_read_uint16(&p_gbr->tile_data.height,    p_obj);
    gbr_read_uint16(&p_gbr->tile_data.count,     p_obj);

    // This is always 4 colors, regardless of Pocket/DMG/CGB/SGB mode
    p_gbr->tile_data.pal_data_size = GBR_TILE_DATA_COLOR_SET_SIZE;
    gbr_read_buf   ( p_gbr->tile_data.color_set, p_obj, p_gbr->tile_data.pal_data_size);

    // Read N Tiles of x Width x Height array of bytes, one byte per tile pixel
    p_gbr->tile_data.tile_data_size = p_gbr->tile_data.width * p_gbr->tile_data.height
                                                             * p_gbr->tile_data.count;
    gbr_read_buf   ( p_gbr->tile_data.tile_list, p_obj, p_gbr->tile_data.tile_data_size);

/*
    int x;
    int y;
    for (y = 0; y < p_gbr->tile_data.height; y++) {
        for (x = 0; x < p_gbr->tile_data.width; x++) {
            printf("%4x", p_gbr->tile_data.tile_list[x + (y * p_gbr->tile_data.width)]);
        }
        printf("\n");
    }
*/




printf("TILE_DATA:\n%s\n %d\n %d\n %d\n %x\n %x\n %x\n %x\n%d\n%d\n",
                                 p_gbr->tile_data.name,
                                 p_gbr->tile_data.width,
                                 p_gbr->tile_data.height,
                                 p_gbr->tile_data.count,
                                 p_gbr->tile_data.color_set[0],
                                 p_gbr->tile_data.color_set[1],
                                 p_gbr->tile_data.color_set[2],
                                 p_gbr->tile_data.color_set[3],
                                 p_gbr->tile_data.pal_data_size,
                                 p_gbr->tile_data.tile_data_size);

  return true;
}



int32_t gbr_object_tile_settings_decode(gbr_record * p_gbr, gbr_file_object * p_obj) {

    if (p_obj->length_bytes != GBR_TILE_SETTINGS_SIZE)
        return false;

    gbr_read_uint16(&p_gbr->tile_settings.tile_id,      p_obj);
    gbr_read_bool  (&p_gbr->tile_settings.simple,       p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.flags,        p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.left_color,   p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.right_color,  p_obj);
    gbr_read_uint16(&p_gbr->tile_settings.split_width,  p_obj);
    gbr_read_uint16(&p_gbr->tile_settings.split_height, p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.split_order,  p_obj); // spec/source shows int32, but file parsing indicates int8
    gbr_read_uint8 (&p_gbr->tile_settings.color_set,    p_obj);
    gbr_read_buf   ( (uint8_t *)p_gbr->tile_settings.bookmarks,
                     p_obj,
                     sizeof(uint16_t) * GBR_TILE_SETTINGS_BOOKMARK_COUNT);
    gbr_read_uint8 (&p_gbr->tile_settings.auto_update,  p_obj);


    // TODO: Consider converting into a function gbr_get_pal_data_size()
    // Set Palette size based on file-wide color-set mode
    switch (p_gbr->tile_settings.color_set) {

        case gbr_color_set_gbc :
        case gbr_color_set_sgb :
            p_gbr->tile_settings.pal_data_size = GBR_TILE_DATA_PALETTE_SIZE_CGB;
            break;

        case gbr_color_set_pocket :
        case gbr_color_set_game_boy :
        default :
            p_gbr->tile_settings.pal_data_size = GBR_TILE_DATA_PALETTE_SIZE_DMG;
            break;
    }


printf("TILE_SETTINGS:\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",
                                 p_gbr->tile_settings.tile_id,
                                 p_gbr->tile_settings.simple,
                                 p_gbr->tile_settings.flags,
                                 p_gbr->tile_settings.left_color,
                                 p_gbr->tile_settings.right_color,
                                 p_gbr->tile_settings.split_width,
                                 p_gbr->tile_settings.split_height,
                                 p_gbr->tile_settings.split_order,
                                 p_gbr->tile_settings.color_set);

  return true;
}


int32_t gbr_object_tile_export_decode(gbr_record * p_gbr, gbr_file_object * p_obj) {

    if (p_obj->length_bytes != GBR_TILE_EXPORT_SIZE)
        return false;

    gbr_read_uint16(&p_gbr->tile_export.tile_id,        p_obj);
    gbr_read_str    (p_gbr->tile_export.file_name,      p_obj, GBR_TILE_EXPORT_FILE_NAME_SIZE);
    gbr_read_uint8 (&p_gbr->tile_export.file_type,      p_obj);
    gbr_read_str    (p_gbr->tile_export.section_name,   p_obj, GBR_TILE_EXPORT_SECTION_NAME_SIZE);
    gbr_read_str    (p_gbr->tile_export.label_name,     p_obj, GBR_TILE_EXPORT_LABEL_NAME_SIZE);
    gbr_read_uint8 (&p_gbr->tile_export.bank,           p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.tile_array,     p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.format,         p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.counter,        p_obj);
    gbr_read_uint16(&p_gbr->tile_export.from,           p_obj);
    gbr_read_uint16(&p_gbr->tile_export.upto,           p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.compression,    p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.include_colors, p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.sgb_palettes,   p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.gbc_palettes,   p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.make_meta_tiles,p_obj);
    gbr_read_uint32(&p_gbr->tile_export.meta_offset,    p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.meta_counter,   p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.split,          p_obj);
    gbr_read_uint32(&p_gbr->tile_export.block_size,     p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.sel_tab,        p_obj);


printf("TILE_EXPORT:\n%d\n%s\n%d\n%s\nTiles: %d - %d\n",
                                 p_gbr->tile_export.tile_id,
                                 p_gbr->tile_export.file_name,
                                 p_gbr->tile_export.file_type,
                                 p_gbr->tile_export.section_name,
                                 p_gbr->tile_export.from,
                                 p_gbr->tile_export.upto);

  return true;
}



int32_t gbr_object_tile_import_decode(gbr_record * p_gbr, gbr_file_object * p_obj) {

    if (p_obj->length_bytes != GBR_TILE_IMPORT_SIZE)
        return false;

            gbr_read_uint16(&p_gbr->tile_import.tile_id,          p_obj);
            gbr_read_str    (p_gbr->tile_import.file_name,        p_obj, GBR_TILE_IMPORT_FILE_NAME_SIZE);
            gbr_read_uint8 (&p_gbr->tile_import.file_type,        p_obj);
            gbr_read_uint16(&p_gbr->tile_import.from_tile,        p_obj);
            gbr_read_uint16(&p_gbr->tile_import.to_tile,          p_obj);
            gbr_read_uint16(&p_gbr->tile_import.tile_count,       p_obj);
            gbr_read_uint8 (&p_gbr->tile_import.color_conversion, p_obj);
            gbr_read_uint32(&p_gbr->tile_import.first_byte,       p_obj);
            gbr_read_uint8 (&p_gbr->tile_import.binary_file_type, p_obj);


printf("TILE_import:\n%d\n%s\n%d\n%d\n%d\n",
                                 p_gbr->tile_import.tile_id,
                                 p_gbr->tile_import.file_name,
                                 p_gbr->tile_import.file_type,
                                 p_gbr->tile_import.from_tile,
                                 p_gbr->tile_import.to_tile);

  return true;
}





int32_t gbr_object_palettes_decode(gbr_record * p_gbr, gbr_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_PALETTES_SIZE_MIN)
        return false;


            gbr_read_uint16(&p_gbr->palettes.id,         p_obj);
            gbr_read_uint16(&p_gbr->palettes.count,      p_obj);
/*
0->248 (darkest -> lightest)

0.. < count | 8
  ClrSets[j] = Colors[j]  // ? = TGBColorSets

    TGBColorType = x[8] [4] [4] = 128
128 bytes = TGBColorSets -> TGBColorType x 8 -> TColor x 4 -> uint8 x 4 (XRGB)

    array[0..7] of TGBColorType;
      TGBColorType    = array [0..3] of TColor;
        TColor : 4 x uint8t;

0.. < count | 4
  color sets


  */
            // CGB palette colors are packed as:
            // * Per Color Entry: RGBX (X is an empty byte)
            // * 4 Colors per Palette
            //* 8 Palettes in total
            gbr_read_buf   ( p_gbr->palettes.colors,     p_obj, GBR_PALETTE_CGB_SETS_SIZE);
            gbr_read_uint16(&p_gbr->palettes.sgb_count,  p_obj);
            gbr_read_buf   ( p_gbr->palettes.sgb_colors, p_obj, GBR_PALETTE_SGB_SETS_ACTUAL_SIZE);


            printf("Palettes:\nid=%d\ncgb_count=%d\nsgb_count=%d\n",
                                             p_gbr->palettes.id,
                                             p_gbr->palettes.count,
                                             p_gbr->palettes.sgb_count);

    return true;
}


int32_t gbr_object_tile_pal_decode(gbr_record * p_gbr, gbr_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_TILE_PAL_SIZE_MIN)
        return false;


            gbr_read_uint16(&p_gbr->tile_pal.id,            p_obj);

            // It's supposed to be tile_pal_count * array of 16 bit ints
            // but instead it seems to be * array of 32 bit ints...?

            // List of palettes that each tile uses, mainly CGB/SGB (Pocket/DMG should always be 1 [palette zero/0])
            gbr_read_uint16(&p_gbr->tile_pal.count,         p_obj);

            p_gbr->tile_pal.color_set_size_bytes = p_gbr->tile_pal.count * GBR_TILE_PAL_COLOR_SET_REC_SIZE;
            gbr_read_buf   ( p_gbr->tile_pal.color_set,
                             p_obj,
                             p_gbr->tile_pal.color_set_size_bytes);


            gbr_read_uint16(&p_gbr->tile_pal.sgb_count,     p_obj);

            p_gbr->tile_pal.sgb_color_set_size_bytes = p_gbr->tile_pal.sgb_count * GBR_TILE_PAL_COLOR_SET_REC_SIZE;
            gbr_read_buf   ( p_gbr->tile_pal.sgb_color_set,
                             p_obj,
                             p_gbr->tile_pal.sgb_color_set_size_bytes);

            gbr_read_padding_bytes(p_obj, GBR_TILE_PAL_UNKNOWN_PADDING);


            printf("gbr:tile pal:\n%d\n%d\n%d\n",
                                 p_gbr->tile_pal.id,
                                 p_gbr->tile_pal.count,
                                 p_gbr->tile_pal.sgb_count);

    return true;
}




// Convert loaded .GBR data to an image
int32_t gbr_convert_tileset_to_image(gbr_record * p_gbr, image_data * p_image, color_data * p_colors) {

    int16_t tile_id;
    int32_t offset;

    p_image->bytes_per_pixel = 1; // TODO: MAKE a #define

    p_image->width  = p_gbr->tile_data.width;
    p_image->height = p_gbr->tile_data.height
                    * p_gbr->tile_data.count;  // TODO: consider a wider image format?

    // Calculate total image area based on
    // tile width and height, and number of tiles
    p_image->size = p_image->width * p_image->height * p_image->bytes_per_pixel;

    // Allocate image buffer
    if (p_image->p_img_data)
        free (p_image->p_img_data);

    p_image->p_img_data = malloc(p_image->size);

    if (p_image->p_img_data) {

        offset = 0;

        // LOAD IMAGE
        // Copy each tile into the image buffer
        for (tile_id=0; tile_id < p_gbr->tile_data.count; tile_id++) {
//            printf("Tile:%d, offset=%d\n", tile_id, offset);
            gbr_tile_get_buf(&p_image->p_img_data[offset],
                             p_gbr,
                             tile_id);

            offset += p_gbr->tile_data.width * p_gbr->tile_data.height * p_image->bytes_per_pixel;
        }

        printf("image:%d x %d (%d x %d # %d) \n", p_image->width, p_image->height,
                                                  p_gbr->tile_data.width,
                                                  p_gbr->tile_data.height,
                                                  p_gbr->tile_data.count);

        // LOAD COLOR MAP
        gbr_load_tileset_palette(p_colors, p_gbr);
        return true;
    }
    else
        return false;
}



int32_t gbr_load_tileset_palette(color_data * p_colors, gbr_record * p_gbr) {

    uint16_t pal_index;
    uint32_t pal_offset;
    uint32_t buf_offset;

    // Note: In CGB/SGB mode, tiles remain stored only referencing 4 colors (0-3), and then
    //       have their palettes remapped on the fly to the associated sub palette (0-7) when rendered.
    //       * In Pocket/DMG mode : Pal will be 0- 3 colors, tiles have colors 0-3 and point to only Palette  0
    //       * In CGB/SGB mode :    Pal will be 0-31 colors, tiles have colors 0-3 and pointing to Palettes 0-7 (0=0-3, 1=4-7, etc, 7=28-31)

    // Make sure there is enough space in destination palette buffer
    if ((p_gbr->tile_data.pal_data_size * COLOR_DATA_BYTES_PER_COLOR) > (COLOR_DATA_PAL_SIZE))
        return false;

    printf("TILE_DATA --> COLOR_SET : %d\n", p_gbr->tile_settings.color_set);

    // Set the destination color palette size based on tile list color set size
    // This will either be 1 palette x 4 colors (Pocket, DMG), or 8 palettes x 4 colors (CGB, SGB?)
    p_colors->color_count = p_gbr->tile_settings.pal_data_size;
    p_colors->size        = p_colors->color_count * COLOR_DATA_BYTES_PER_COLOR;


    printf("COLOR: size=%d\n", p_colors->color_count);

    buf_offset = 0;

    // Load the palette colors for each entry in the tile list color set
    for(pal_index = 0; pal_index < p_colors->color_count; pal_index++) {

        // Find the tile palette entry for the tile list color
        pal_offset = pal_index * GBR_PALETTE_TCOLOR_SIZE;

        // Load the color data into the destination palette
        p_colors->pal[buf_offset++] = p_gbr->palettes.colors[pal_offset + 0]; // R.0
        p_colors->pal[buf_offset++] = p_gbr->palettes.colors[pal_offset + 1]; // G.1
        p_colors->pal[buf_offset++] = p_gbr->palettes.colors[pal_offset + 2]; // B.2


        printf(" --COLORS %d: %d) %2x, %2x, %2x, \n", pal_index, pal_offset,
                                                p_colors->pal[buf_offset-3],
                                                p_colors->pal[buf_offset-2],
                                                p_colors->pal[buf_offset-1]);
    }

    return true;
}
