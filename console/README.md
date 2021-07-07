png2gbtiles: a standalone, command line tool for converting png tilesets and tilemaps to .gbr and .gbm (and possibly .c) files
===========

Builds available for Linux and Windows

See notes about Usage & Cautions in the main project's [Readme](https://github.com/bbbbbr/gimp-tilemap-gb/blob/master/README.md)

Extra features (vs gimp-plugin)
- Remapping image to custom user palette (supports indexed and 24/32-bit color PNGs)
  - Supports sub-palettes (eg. 8 x 4 color palettes as in CGB)


```
Usage
   png2gbtiles input_file.png -gbr|-gbm|-csource [options] [output_file]

Options

  -g          Force DMG color mode (4 colors or less only)
  -c          Force CGB color mode (up to 32 colors)

  -d          Turn OFF Map tile deduplication of tile PATTERN (.gbm only)
  -f          Turn OFF Map tile deduplication of FLIP X/Y (.gbm only)
  -p          Turn OFF Map tile deduplication of ALTERNATE PALETTE (.gbm only)

  -i          Ignore Palette Errors (CGB will use highest guessed palette #)
  -pal=[file] Remap png to palette (pngs allowed: index and 24/32 bit RGB)

  -q          Quiet, suppress all output
  -e          Errors only, suppress all non-error output
  -v          Verbose output during conversion

Examples
   png2gbtiles spritesheet.png -gbr spritesheet.gbr
   png2gbtiles worldmap.png -gbm -d -f -p worldmap.gbm
   png2gbtiles worldmap.png -gbm 
   png2gbtiles a_24bit_map.png -gbm -pal=colors_to_use.pal
```
