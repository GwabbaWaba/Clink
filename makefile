BIN_DIR=./bin
MODS_DIR=./mods
BASE_MODS_DIR=./base_mods

all: directories out base_mods mods
directories: $(BIN_DIR) $(MODS_DIR) $(BASE_MODS_DIR)
$(BIN_DIR) $(MODS_DIR) $(BASE_MODS_DIR):
	@if [ ! -d $@ ]; then mkdir -p $@; fi;

test_mod=mods/test.so
clink=base_mods/clink.so
mods: $(test_mod)
base_mods: $(clink)

CPP_FLAGS =-std=c++20
PKG   =
LIBS  =-ldl -lraylib -lm

ZIG_O =bin/zig.o
DEFINES =
BUILD_MODE =-DDEBUG -DPRETTY_LOGS

out: makefile $(ZIG_O) $(wildcard src/*) $(base_mods)
	zig c++ $(CPP_FLAGS) $(BUILD_MODE) $(DEFINES) src/*.c?? $(ZIG_O) $(PKG) $(LIBS) -o out

$(clink): makefile $(wildcard base_mods_src/clink/*) $(wildcard src/*.h*)
	zig c++ $(CPP_FLAGS) $(BUILD_MODE) $(DEFINES) --shared base_mods_src/clink/*.c?? $(ZIG_O) $(PKG) $(LIBS) -o $(clink)

$(test_mod): makefile $(wildcard mods_src/test/*) $(wildcard src/*.h*)
	zig c++ $(CPP_FLAGS) $(BUILD_MODE) $(DEFINES) --shared mods_src/test/*.c?? $(ZIG_O) $(PKG) $(LIBS) -o $(test_mod)

$(ZIG_O): makefile $(wildcard src/*.zig)
	zig build-obj src/zig.zig -lc++ -I. -femit-bin=$(ZIG_O)
	rm bin/zig.o.o

.PHONY: all mods base_mods
