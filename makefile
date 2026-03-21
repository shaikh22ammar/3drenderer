.PHONY: clean tags

CC = clang
CFLAGS = -Wall -std=c11
CFLAGS += -Wdouble-promotion -Wfloat-conversion
CFLAGS += $(shell pkg-config --cflags sdl3)

LFLAGS = 
LFLAGS += $(shell pkg-config --libs sdl3)
LFLAGS += -lm

run: render.out
	./render.out

debug: CFLAGS += -g -O0 -DDEBUG
debug: render.out

render.out: src/main.c src/display.* src/vector.* src/mesh.*
	$(CC) src/*.c -o render.out $(CFLAGS) $(LFLAGS)

clean: 
	rm render.out
	rm src/*.o

tags:
	ctags -R --c-kinds=+tfvse \
		--languages=C \
		--langmap=C:.c.h \
		-f tags \
		. \
		/opt/homebrew/include/SDL3/
