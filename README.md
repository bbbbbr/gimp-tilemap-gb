Gimp Plugin: tilemap generator and exporter for Game Boy / GBDK & ZGB
===========

GIMP plugin for importing & exporting Game Boy game tilemaps and tilesets.

You can create a level map in Gimp (or other image editors) then export it to either C Source files for the GBDK, or export as .gbr / .gbm for editing and further work in either GBTD or GBMB. You can also import from .gbr and .gbm files, and re-export to them (tile order may change and not all app prefs may survive the import/re-export trip though).

*This plugin is functional, but not polished. Don't be surprised to find some rough edges or annoyances.


![Opening a Game Boy Map Builder map as an image in GIMP](https://raw.githubusercontent.com/bbbbbr/gimp-tilemap/master/info/gimp-tilemap-plugin-screenshot.png)


Features
 * Tile deduplication

Tile Map export formats:
* GBM (Game Boy Map Builder http://www.devrs.com/gb/hmgd/gbmb.html )
* C Source (GBDK / ZGB)

Tile Set export formats:
 * C (GBDK / ZGB)
 * GBR (Game Boy Tile Designer http://www.devrs.com/gb/hmgd/gbtd.html )
 
Tile Set & Map import formats:
 * GBR
 * GBM

Supported formats:
 * GB 2bpp (CGB not supported at this time)

OS binaries available for:
 * Linux (GIMP 2.8+)
 * Windows (GIMP 2.10.12+)


## Usage:
 * Work on an 8x8 grid. It will save you time, trouble and ROM size later on.
 * Exported C Source / GBM Map Tilesets *always* use a tile size of 8x8 pixels, and the source image should be an even multiple of the tile size
 * Exported GBR tilesets should have an image width of 8, 16 or 32 pixels. Tile size width and height will auto set to be the same as the image width. This means 8x16 is not currently supported. (Maybe a dialog selecting export size later).
 * Exported GBR should have no more than 4 colors, for now.
 * Metadata stored in the GBR / GBM files, such as recently used files, etc is not currently preserved when exporting back out. (Maybe coming later, it's do-able and partially in there).
 * If you export using this path: GIMP -> GBTD or GBMB -> C source, then you'll need to set up the export options for GBTD (path, tiles to export, etc) and GBMB (path, location format, etc).

## Quick instructions:

Native compile/install on Linux using below.

```
gimptool-2.0 --install file-tilemap-export.c
    or
 make (and then copy to your GIMP plugin folder, depends on version)

Plug-in folder locations:
 Linux: ~/.gimp-2.8/plug-ins
 Windows: C:\Program Files\GIMP 2\lib\gimp\2.0\plug-ins

```

## Requirements:
* Image must be indexed color and not have more colors than output format supports

## Known limitations & Issues:
* GBMB Export does not yet auto-populate the Location Format, so it must be done manually.

## GIMP usage hints:
