all: ./bin ./base_mods ./mods out base_mods mods

./bin:
	mkdir -p ./bin
./mods:
	mkdir -p ./mods
./base_mods:
	mkdir -p ./base_mods


test_mod=mods/test.so
clink=base_mods/clink.so
mods: $(test_mod)
base_mods: $(clink)

CPP_FLAGS =-std=c++20
PKG   =
LIBS  =-ldl -lraylib -lm

ZIG_O =bin/zig.o
DEFINES =
BUILD_MODE =-DDEBUG

out: makefile $(ZIG_O) $(wildcard src/*) $(base_mods)
	zig c++ $(CPP_FLAGS) $(BUILD_MODE) $(DEFINES) src/*.c?? $(ZIG_O) $(PKG) $(LIBS) -o out

$(clink): makefile $(wildcard base_mods_src/clink/*) $(wildcard src/*.h*)
	zig c++ $(CPP_FLAGS) $(BUILD_MODE) $(DEFINES) --shared base_mods_src/clink/*.c?? $(ZIG_O) $(PKG) $(LIBS) -o $(clink)

$(test_mod): makefile $(wildcard mods_src/test/*) $(wildcard src/*.h*)
	zig c++ $(CPP_FLAGS) $(BUILD_MODE) $(DEFINES) --shared mods_src/test/*.c?? $(ZIG_O) $(PKG) $(LIBS) -o $(test_mod)

$(ZIG_O): makefile $(wildcard src/*.zig)
	zig build-obj src/zig.zig -lc++ -I. -femit-bin=$(ZIG_O)
	rm bin/zig.o.o
