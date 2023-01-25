
DEL = rm -f
SRCDIR = src
LODEPNGDIR = lodepng
TILEMAPGBDIR = ../src
OUTDIR = bin
INCS = -I"$(SRCDIR)" -I"$(LODEPNGDIR)" -I"$(TILEMAPGBDIR)"
CFLAGS = $(INCS) -Wno-format-truncation

BIN = png2gbtiles
BIN_WIN = $(BIN).exe

# Add all c source files
CALL = $(wildcard src/*.c) \
		$(wildcard lodepng/*.c) \
		$(wildcard ../src/*.c)
# Remove some files that won't/can't be used
CEXCLUDE = $(TILEMAPGBDIR)/tilemap_write.c $(TILEMAPGBDIR)/tilemap_read.c $(TILEMAPGBDIR)/file-tilemap.c $(TILEMAPGBDIR)/export-dialog.c
CFILES = $(filter-out $(CEXCLUDE),$(CALL))
COBJ = $(CFILES:.c=.o)

all: linux

info:
	@echo "--> Please specify target, 'make linux' or 'make wincross' (MinGW Windows build)"


# Linux MinGW build for Windows
# -lb for math.h
# (static linking to avoid DLL dependencies)
wincross: TARGET=i686-w64-mingw32
wincross: CC = $(TARGET)-g++
wincross: LDFLAGS = -s -static -lm
wincross: $(COBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(BIN_WIN) $^ $(LDFLAGS)

# Linux build
# -lb for math.h
macos: linux
linux : CC = gcc
linux : LDFLAGS = -s -lm
linux: $(COBJ)
	mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -o $(BIN) $^ $(LDFLAGS)

cleanobj:
	$(DEL) $(COBJ)

clean:
	$(DEL) $(COBJ) $(BIN) $(BIN_WIN)

macoszip: macos
	mkdir -p bin
	mkdir -p bin/macos
	strip $(BIN)
	zip $(BIN)-macos.zip $(BIN)
	mv $(BIN)-macos.zip bin
	cp $(BIN) bin/macos

linuxzip: linux
	mkdir -p bin
	mkdir -p bin/linux
	strip $(BIN)
	zip $(BIN)-linux.zip $(BIN)
	mv $(BIN)-linux.zip bin
	cp $(BIN) bin/linux

wincrosszip: wincross
	mkdir -p bin
	mkdir -p bin/windows
	strip $(BIN_WIN)
	zip $(BIN)-windows.zip $(BIN_WIN)
	mv $(BIN)-windows.zip bin
	cp $(BIN_WIN) bin/windows

package:
	${MAKE} clean
	${MAKE} wincrosszip
	${MAKE} clean
	${MAKE} linuxzip
	${MAKE} clean

