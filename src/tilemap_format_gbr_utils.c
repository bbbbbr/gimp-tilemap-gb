//
//   asdasdasd
//

// Reference :
// * https://github.com/bashaus/gbtiles
// * http://www.devrs.com/gb/hmgd/gbtd.html


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>


#define PASCAL_OBJECT_MAX_SIZE 65535

#define GBR_PRODUCER_NAME_SIZE    30
#define GBR_PRODUCER_VERSION_SIZE 10
#define GBR_PRODUCER_INFO_SIZE    80
#define GBR_PRODUCER_SIZE  GBR_PRODUCER_NAME_SIZE + GBR_PRODUCER_VERSION_SIZE + GBR_PRODUCER_INFO_SIZE

#define GBR_PRODUCER_NAME_SIZE_STR    GBR_PRODUCER_NAME_SIZE    + 1 // space for trailing \0
#define GBR_PRODUCER_VERSION_SIZE_STR GBR_PRODUCER_VERSION_SIZE + 1
#define GBR_PRODUCER_INFO_SIZE_STR    GBR_PRODUCER_INFO_SIZE    + 1

#define GBR_TILE_DATA_NAME_SIZE       30
#define GBR_TILE_DATA_NAME_SIZE_STR   GBR_TILE_DATA_NAME_SIZE + 1
#define GBR_TILE_DATA_COLOLR_SET_SIZE 4
#define GBR_TILE_DATA_SIZE_MIN        GBR_TILE_DATA_NAME_SIZE + 2 + 2 + 2 + GBR_TILE_DATA_COLOLR_SET_SIZE

#define GBR_TILE_SETTINGS_SIZE        19

#define GBR_TILE_EXPORT_FILE_NAME_SIZE    128
#define GBR_TILE_EXPORT_SECTION_NAME_SIZE 20
#define GBR_TILE_EXPORT_LABEL_NAME_SIZE   20

#define GBR_TILE_EXPORT_FILE_NAME_SIZE_STR    GBR_TILE_EXPORT_FILE_NAME_SIZE + 1
#define GBR_TILE_EXPORT_SECTION_NAME_SIZE_STR GBR_TILE_EXPORT_SECTION_NAME_SIZE + 1
#define GBR_TILE_EXPORT_LABEL_NAME_SIZE_STR   GBR_TILE_EXPORT_LABEL_NAME_SIZE + 1

#define GBR_TILE_EXPORT_SIZE                  27 + GBR_TILE_EXPORT_FILE_NAME_SIZE + GBR_TILE_EXPORT_SECTION_NAME_SIZE + GBR_TILE_EXPORT_LABEL_NAME_SIZE


#define GBR_TILE_IMPORT_FILE_NAME_SIZE     128
#define GBR_TILE_IMPORT_FILE_NAME_SIZE_STR GBR_TILE_IMPORT_FILE_NAME_SIZE + 1
#define GBR_TILE_IMPORT_SIZE               15 + GBR_TILE_IMPORT_FILE_NAME_SIZE

#define GBR_PALETTES_SIZE_MIN 6
#define GBR_TILE_PAL_SIZE_MIN 6


enum gbr_tilemap_layer {
    gbr_bkg    = 0x00,
    gbr_win    = 0x80,
    gbr_sprite = 0x00
};


enum gbr_tileset_colorset {
    gbr_color_set_pocket   = 0,
    gbr_color_set_game_boy = 1,
    gbr_color_set_gbc      = 2,
    gbr_color_set_sgb      = 3
};

enum gbr_object_types {
    gbr_producer      = 0x01,
    gbr_tile_data     = 0x02,
    gbr_tile_settings = 0x03,
    gbr_tile_export   = 0x04,
    gbr_tile_import   = 0x05,
    gbr_palettes      = 0x0D,
    gbr_tile_pal      = 0x0E,
    gbr_deleted       = 0xFF
};


enum gbr_tileset_sgbpalettes {
    none               = 0,
    constant_per_entry = 1,
    per_entry_2_bits   = 2, // 2_bits_per_entry
    per_entry_4_bits   = 3, // 4_bits_per_entry
    per_entry_1_byte   = 4  // 1_byte_per_entry
};


enum gbr_tileset_sgbpalettes {
    lrtb       = 0,
    horizontal = 0,
    tblr       = 1,
    vertical   = 1
};
    module TileSet
      SPLIT_ORDER = {
        :lrtb       => 0,
        :horizontal => 0,

        :tblr       => 1,
        :vertical   => 1
      }

typedef struct {
    int8_t name[GBR_PRODUCER_NAME_SIZE_STR];
    int8_t version[GBR_PRODUCER_VERSION_SIZE_STR];
    int8_t info[GBR_PRODUCER_INFO_SIZE_STR];
} gpr_producer;


typedef struct {
    int8_t     name[GBR_TILE_DATA_NAME_SIZE_STR];
    uint16_t   width;
    uint16_t   height;
    uint16_t   count;
    uint8_t    color_set[GBR_TILE_DATA_COLOLR_SET_SIZE];
    uint8_t    p_data[PASCAL_OBJECT_MAX_SIZE];
    uint32_t   data_size;
} gpr_tile_data;


typedef struct {
    uint16_t  tile_id;
    uint8_t   simple;
    uint8_t   flags;
    uint8_t   left_color;
    uint8_t   right_color;
    uint16_t  split_width;
    uint16_t  split_height;
    uint32_t  split_order;
    uint8_t   color_set;
    // bookmarks    Word(3)
    // auto_update  Boolean
} gpr_tile_settings;



typedef struct {
    uint16_t   tile_id;
    int8_t     file_name[GBR_TILE_EXPORT_FILE_NAME_SIZE_STR];
    uint8_t    file_type;
    int8_t     section_name[GBR_TILE_EXPORT_SECTION_NAME_SIZE_STR];
    int8_t     label_name[GBR_TILE_EXPORT_LABEL_NAME_SIZE_STR];
    uint8_t    bank;
    uint8_t    tile_array;
    uint8_t    format;
    uint8_t    counter;
    uint16_t   from;
    uint16_t   upto;
    uint8_t    compression;
    uint8_t    include_colors;
    uint8_t    sgb_palettes;
    uint8_t    gbc_palettes;
    uint8_t    make_meta_tiles;
    uint32_t   meta_offset;
    uint8_t    meta_counter;
    uint8_t    split;
    uint32_t   block_size;
    uint8_t    sel_tab;
} gpr_tile_export;


typedef struct {
    uint16_t  tile_id;
    int8_t    file_name[GBR_TILE_IMPORT_FILE_NAME_SIZE];
    uint8_t   file_type;
    uint16_t  from_tile;
    uint16_t  to_tile;
    uint16_t  tile_count;
    uint8_t   color_conversion;
    uint32_t  first_byte;
    uint8_t   binary_file_type;

} gpr_tile_import;


typedef struct {
    uint16_t  id;
    uint16_t  count;
    uint8_t   colors[PASCAL_OBJECT_MAX_SIZE];
    uint16_t  sgb_count;
    uint8_t   sgb_colors[PASCAL_OBJECT_MAX_SIZE];
} gpr_palettes;

typedef struct {
    uint16_t  id;
    uint16_t  count;
    uint8_t   color_set[PASCAL_OBJECT_MAX_SIZE];
    uint16_t  sgb_count;
    uint8_t   sgb_color_set[PASCAL_OBJECT_MAX_SIZE];
} gpr_tile_pal;



typedef struct {
    gpr_producer      producer;
    gpr_tile_data     tile_data;
    gpr_tile_settings tile_settings;
    gpr_tile_export   tile_export;
    gpr_tile_import   tile_import;
    gpr_palettes      palettes;
    gpr_tile_pal      tile_pal;
} gbr_record;



typedef struct {
    uint16_t   type;
    uint16_t   id;
    uint32_t   length_bytes;
    uint32_t   offset;
    uint8_t  * p_data;
} pascal_file_object;


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
}

void gbr_read_buf(int8_t * p_dest_buf, pascal_file_object * p_obj, uint32_t n_bytes) {

    // Copy string, add terminator, move offset to next data
    memcpy(p_dest_buf, &p_obj->p_data[ p_obj->offset ], n_bytes);
    p_obj->offset += n_bytes;
}

void gbr_read_uint32(uint32_t * p_dest_val, pascal_file_object * p_obj) {

    memcpy(p_dest_val, &p_obj->p_data[ p_obj->offset ], sizeof(uint32_t));
    p_obj->offset += sizeof(uint32_t);
}

void gbr_read_uint16(uint16_t * p_dest_val, pascal_file_object * p_obj) {

    memcpy(p_dest_val, &p_obj->p_data[ p_obj->offset ], sizeof(uint16_t));
    p_obj->offset += sizeof(uint16_t);
}

void gbr_read_uint8(uint8_t * p_dest_val, pascal_file_object * p_obj) {

    memcpy(p_dest_val, &p_obj->p_data[ p_obj->offset ], sizeof(uint8_t));
    p_obj->offset += sizeof(uint8_t);
}



int32_t gbr_object_producer_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

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


int32_t gbr_object_tile_data_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_TILE_DATA_SIZE_MIN)
        return false;

    gbr_read_str   ( p_gbr->tile_data.name,      p_obj, GBR_TILE_DATA_NAME_SIZE);
    gbr_read_uint16(&p_gbr->tile_data.width,     p_obj);
    gbr_read_uint16(&p_gbr->tile_data.height,    p_obj);
    gbr_read_uint16(&p_gbr->tile_data.count,     p_obj);
    gbr_read_buf   ( p_gbr->tile_data.color_set, p_obj, GBR_TILE_DATA_COLOLR_SET_SIZE);
    // TODO: How much to read here? does it include the color set or not?
    // Save the remaining bytes to a buffer
    p_gbr->tile_data.data_size = p_obj->length_bytes - p_obj->offset;
    gbr_read_buf(p_gbr->tile_data.p_data, p_obj, p_gbr->tile_data.data_size);


printf("TILE_DATA:\n%s\n %d\n %d\n %d\n %x\n %x\n %x\n %x\n%d\n",
                                 p_gbr->tile_data.name,
                                 p_gbr->tile_data.height,
                                 p_gbr->tile_data.width,
                                 p_gbr->tile_data.count,
                                 p_gbr->tile_data.color_set[0],
                                 p_gbr->tile_data.color_set[1],
                                 p_gbr->tile_data.color_set[2],
                                 p_gbr->tile_data.color_set[3],
                                 p_gbr->tile_data.data_size);

  return true;
}



int32_t gbr_object_tile_settings_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes != GBR_TILE_SETTINGS_SIZE)
        return false;

    gbr_read_uint16(&p_gbr->tile_settings.tile_id,      p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.simple,       p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.flags,        p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.left_color,   p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.right_color,  p_obj);
    gbr_read_uint16(&p_gbr->tile_settings.split_width,  p_obj);
    gbr_read_uint16(&p_gbr->tile_settings.split_height, p_obj);
    gbr_read_uint32(&p_gbr->tile_settings.split_order,  p_obj); // Should this be uint16? = 4294901760
    gbr_read_uint8 (&p_gbr->tile_settings.color_set,    p_obj);
    // TODO: How much to read here? does it include the color set or not?
    // Save the remaining bytes to a buffer
    gbr_read_buf   ( p_gbr->tile_data.p_data,    p_obj, p_obj->length_bytes - p_obj->offset);


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


int32_t gbr_object_tile_export_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

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


printf("TILE_EXPORT:\n%d\n%s\n%d\n%s\n%s\n",
                                 p_gbr->tile_export.tile_id,
                                 p_gbr->tile_export.file_name,
                                 p_gbr->tile_export.file_type,
                                 p_gbr->tile_export.section_name,
                                 p_gbr->tile_export.section_name);

  return true;
}



int32_t gbr_object_tile_import_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

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




int32_t gbr_object_palettes_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_PALETTES_SIZE_MIN)
        return false;


            gbr_read_uint16(&p_gbr->palettes.id,         p_obj);
            gbr_read_uint16(&p_gbr->palettes.count,      p_obj);
            gbr_read_buf   ( p_gbr->palettes.colors,     p_obj, p_gbr->palettes.count); // TODO: this probably isn't working right
            gbr_read_uint16(&p_gbr->palettes.sgb_count,  p_obj);
            gbr_read_buf   ( p_gbr->palettes.sgb_colors, p_obj, p_gbr->palettes.sgb_count); // TODO: this probably isn't working right

/*
// TODO
0
OBJ: type=13, id=5, size=262 <--- -6 = 256 for palettes
OBJ: type=0d, id=05, size=0106
gbr_palettes
PALEttes:
1
8
29984

*/

printf("PALEttes:\n%d\n%d\n%d\n",
                                 p_gbr->palettes.id,
                                 p_gbr->palettes.count,
                                 p_gbr->palettes.sgb_count);

  return true;
}


/*
   object = GBTiles::GBR::TileSet::Objects::TilePal.new

            gbr_read_uint16(&p_gbr->tile_export.id     ,        p_obj);
            gbr_read_uint16(&p_gbr->tile_export.count  ,        p_obj);
            object.color_set      = src.slice!(0, object.count)
            gbr_read_uint16(&p_gbr->tile_export.sgb_count ,        p_obj);
            object.sgb_color_set  = src.slice!(0, object.sgb_count)
*/



int32_t gbr_object_tile_pal_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_TILE_PAL_SIZE_MIN)
        return false;


            gbr_read_uint16(&p_gbr->tile_pal.id,            p_obj);
            gbr_read_uint16(&p_gbr->tile_pal.count,         p_obj);
            gbr_read_buf   ( p_gbr->tile_pal.color_set,     p_obj, p_gbr->tile_pal.count); // TODO: this probably isn't working right
            gbr_read_uint16(&p_gbr->tile_pal.sgb_count,     p_obj);
            gbr_read_buf   ( p_gbr->tile_pal.sgb_color_set, p_obj, p_gbr->tile_pal.sgb_count); // TODO: this probably isn't working right


printf("tile pal:\n%d\n%d\n%d\n",
                                 p_gbr->tile_pal.id,
                                 p_gbr->tile_pal.count,
                                 p_gbr->tile_pal.sgb_count);

  return true;
}


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
                    case gbr_producer: printf("gbr_producer\n");
                                       gbr_object_producer_decode(&gbr, &obj);
                                       break;

                    case gbr_tile_data: printf("gbr_tile_data\n");
                                        gbr_object_tile_data_decode(&gbr, &obj);
                                        break;

                    case gbr_tile_settings: printf("gbr_tile_settings\n");
                                            gbr_object_tile_settings_decode(&gbr, &obj);
                                            break;

                    case gbr_tile_export: printf("gbr_tile_export\n");
                                          gbr_object_tile_export_decode(&gbr, &obj);
                                          break;

                    case gbr_tile_import: printf("gbr_tile_import\n");
                                          gbr_object_tile_import_decode(&gbr, &obj);
                                          break;

                    case gbr_palettes: printf("gbr_palettes\n");
                                       gbr_object_palettes_decode(&gbr, &obj);
                                       break;

                    case gbr_tile_pal: printf("gbr_tile_pal\n");
                                       gbr_object_tile_pal_decode(&gbr, &obj);
                                       break;

                    case gbr_deleted: printf("gbr_deleted\n");
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


/*

GBR FILE


 def initialize
          @tile_set = GBTiles::GBR::TileSet::TileSet.new
        end

        def self.open file
          import = GBTiles::GBR::Import::GBRFile.new

          # Check to see if this is a valid file type
          if (file.read(3) != "GBO") then
            raise IOError, "Not a valid file (expected: GBO)"
          end

          # Version number
          import.version = file.read(1)

          # For each object
          while !file.eof?
            object_type = GBTiles::DataType.word(file.read(2))
            object_id   = GBTiles::DataType.word(file.read(2))
            object_len  = GBTiles::DataType.long(file.read(4))
            object_data = file.read(object_len)

            case GBTiles::GBR::TileSet::OBJECT_TYPE.key(object_type)
            when :producer
              object = GBTiles::GBR::TileSet::Objects::Producer.initFromBitString(object_data)

            when :tile_data
              object = GBTiles::GBR::TileSet::Objects::TileData.initFromBitString(object_data)

            when :tile_settings
              object = GBTiles::GBR::TileSet::Objects::TileSettings.initFromBitString(object_data)

            when :tile_export
              object = GBTiles::GBR::TileSet::Objects::TileExport.initFromBitString(object_data)

            when :tile_import
              object = GBTiles::GBR::TileSet::Objects::TileImport.initFromBitString(object_data)

            when :palettes
              object = GBTiles::GBR::TileSet::Objects::Palettes.initFromBitString(object_data)

            when :tile_pal
              object = GBTiles::GBR::TileSet::Objects::TilePal.initFromBitString(object_data)

            else
              object = GBTiles::GBR::TileSet::Objects::Unknown.new object_type
              object.object_data = object_data

            end

            object.object_id = object_id

            import.tile_set.objects << object
          end

          import
        end
      end

*/





/*

GBM FILE

module GBTiles
  module GBM
    module Import
      class GBMFile

        OBJECT_MARKER = "HPJMTL"

        attr_accessor :version
        attr_accessor :map_set

        def initialize
          @map_set = GBTiles::GBM::Map::MapSet.new
        end

        def self.open file
          import = GBTiles::GBM::Import::GBMFile.new

          # Check to see if this is a valid file type
          if (file.read(3) != "GBO") then
            raise IOError, "Not a valid file (expected: GBO)"
          end

          # Version number
          import.version = file.read(1)

          if import.version != "1" then
            raise IOError, "Cannot import version #{import.version}"
          end

          # For each object
          while !file.eof?
            # Check the marker
            marker = GBTiles::DataType.string(file.read(6))

            if !OBJECT_MARKER.eql? marker then
              raise "Malformed file, expected marker #{OBJECT_MARKER}, got #{marker}"
            end

            object_type = GBTiles::DataType.word(file.read(2))
            object_id   = GBTiles::DataType.word(file.read(2))
            master_id   = GBTiles::DataType.word(file.read(2))
            object_crc  = GBTiles::DataType.long(file.read(4))
            object_len  = GBTiles::DataType.long(file.read(4))
            object_data = file.read(object_len)

            case GBTiles::GBM::Map::OBJECT_TYPE.key(object_type)
            when :producer
              object = GBTiles::GBM::Map::Objects::Producer.initFromBitString object_data

            when :map
              object = GBTiles::GBM::Map::Objects::Map.initFromBitString object_data

            when :map_tile_data
              object = GBTiles::GBM::Map::Objects::MapTileData.initFromBitString object_data

            when :map_settings
              object = GBTiles::GBM::Map::Objects::MapSettings.initFromBitString object_data

            when :map_export_settings
              object = GBTiles::GBM::Map::Objects::MapExportSettings.initFromBitString object_data

            else
              object = GBTiles::GBM::Map::Objects::Unknown.new object_type
              object.object_data = object_data

            end

            object.object_id = object_id
            object.master_id = master_id

            import.map_set.objects << object
          end

          import
        end
      end
    end
  end
end

*/