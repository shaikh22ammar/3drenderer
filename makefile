.PHONY: clean tags

CC = clang
CFLAGS = -Wall -std=c11
CFLAGS += $(shell pkg-config --cflags sdl3)

LFLAGS = 
LFLAGS += $(shell pkg-config --libs sdl3)
LFLAGS += -lm

run: renderer.out
	./render.out

renderer.out: src/display.c src/main.c
	$(CC) src/main.c src/display.c -o render.out $(CFLAGS) $(LFLAGS)


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
