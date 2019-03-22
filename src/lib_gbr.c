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