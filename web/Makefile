# @file Makefile
# @brief Build web version of game project.
# @author aurelien.esnard@u-bordeaux.fr
# @copyright University of Bordeaux. All rights reserved, 2022.

#ALL: game.js game.wasm

ALL: game.js

LIBSRC  := $(wildcard src/*.c)
LIBOBJ  := $(LIBSRC:.c=.o)

game.js: wrapper.o libgame.a
	emcc $^ -o game.js \
		-s MODULARIZE=1 \
		-s EXPORT_ES6=1 \
		-s ENVIRONMENT=web \
		-s ALLOW_MEMORY_GROWTH=1 \
		-s EXPORTED_RUNTIME_METHODS=ccall,cwrap

%.o: %.c
	emcc -I src -c $< -o $@

libgame.a: $(LIBOBJ)
	emar rcs libgame.a $^

clean:
	rm -f *.o src/*.o game.js game.wasm libgame.a