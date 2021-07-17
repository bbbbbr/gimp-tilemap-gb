png2gbtiles: a standalone, command line tool for converting png tilesets and tilemaps to .gbr and .gbm (and possibly .c) files
===========

Builds available for Linux and Windows

See notes about Usage & Cautions in the main project's [Readme](https://github.com/bbbbbr/gimp-tilemap-gb/blob/master/README.md)


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
  -pal=[file] Remap png to palette (pngs allowed: index or 24/32 bit RGB)

  -var=[name]    Base name to use for export variables (otherwise filename)
  -bank=[num]    Set bank number for all output modes
  -tileorg=[num] Tile index offset for maps (instead of zero)
  -tilesz=[size] Tile size (8x8, 8x16, 16x16, 32x32) 

  -q          Quiet, suppress all output
  -e          Errors only, suppress all non-error output
  -v          Verbose output during conversion

Examples
   png2gbtiles spritesheet.png -gbr spritesheet.gbr
   png2gbtiles worldmap.png -gbm -d -f -p worldmap.gbm
   png2gbtiles worldmap.png -gbm 
   png2gbtiles worldmap.png -gbm -c -pal=mypal.pal -bank=4 -tileorg=64
Remap Palette format: RGB in hex text, 1 color per line (ex: FF0080)

```

The remap palettes are text format `#RRGGBB`.

For example, a 4-color DMG palette:
```
#c6d783
#8ecf53
#2d632f
#11380f
```

For CGB palettes
  * There can be up to 32 colors total, organized as consecutive groups of 4-color sub-palettes. 
  * The total number of colors must be an even multiple of 4.
  * A given tile can only use one 4-color subpalette.


