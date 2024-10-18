all: out mods

test_mod=mods/test.so
mods: $(test_mod)

PKG   =
LIBS  =-ldl -lraylib -lm

ZIG_O =bin/zig.o

out: makefile $(ZIG_O) $(wildcard src/*) 
	zig c++ src/*.c?? $(ZIG_O) $(FLAGS) $(PKG) $(LIBS) -o out

$(test_mod): makefile $(wildcard mods_src/test/*)
	zig c++ --shared mods_src/test/*.c?? $(ZIG_O) $(FLAGS) $(PKG) $(LIBS) -o $(test_mod)

$(ZIG_O): makefile $(wildcard src/*.zig)
	zig build-obj src/zig.zig -lc++ -I. $(FLAGS) -femit-bin=$(ZIG_O)
	rm bin/zig.o.o
