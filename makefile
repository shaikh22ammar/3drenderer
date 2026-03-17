CC = clang -Wall -std=c99
CFLAGS = -Wall -std=c99
CFLAGS += $(shell pkg-config --cflags sdl3)

LFLAGS = 
LFLAGS += $(shell pkg-config --libs sdl3)

main:
	$(CC) src/main.c -o render.out $(CFLAGS) $(LFLAGS)

run:
	./render.out

clean: 
	rm render.out
