
DEL = rm -f
SRCDIR = src
LODEPNGDIR = lodepng
TILEMAPGBDIR = ../src
OUTDIR = bin
INCS = -I"$(SRCDIR)" -I"$(LODEPNGDIR)" -I"$(TILEMAPGBDIR)"
CFLAGS = $(INCS)

# Add all c source files
CALL = $(wildcard src/*.c) \
		$(wildcard lodepng/*.c) \
		$(wildcard ../src/*.c)
# Remove some files that won't/can't be used
CEXCLUDE = $(TILEMAPGBDIR)/tilemap_write.c $(TILEMAPGBDIR)/tilemap_read.c $(TILEMAPGBDIR)/file-tilemap.c $(TILEMAPGBDIR)/export-dialog.c
CFILES = $(filter-out $(CEXCLUDE),$(CALL))
COBJ = $(CFILES:.c=.o)


info:
	@echo "--> Please specify target, 'make png2gbtiles' (Linux build) or 'make png2gbtiles.exe' (MinGW Windows build)"


# Linux MinGW build for Windows
# -lb for math.h
# (static linking to avoid DLL dependencies)
png2gbtiles.exe : TARGET=i686-w64-mingw32
png2gbtiles.exe : CC = $(TARGET)-g++
png2gbtiles.exe : LDFLAGS = -s -static -lm
png2gbtiles.exe: $(COBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$@ $^ $(LDFLAGS)

# Linux build
# -lb for math.h
png2gbtiles : CC = gcc
png2gbtiles : LDFLAGS = -s -lm
png2gbtiles: $(COBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(OUTDIR)/$@ $^ $(LDFLAGS)

cleanobj:
	$(DEL) $(COBJ)

clean:
	$(DEL) $(COBJ) $(OUTDIR)/png2gbtiles.exe $(OUTDIR)/png2gbtiles

