CC = clang -Wall -std=c99
CFLAGS = -Wall -std=c99
CFLAGS += $(shell pkg-config --cflags sdl3)

LFLAGS = 
LFLAGS += $(shell pkg-config --libs sdl3)

src/main: src/display.o src/main.c
	$(CC) src/main.c src/display.o -o render.out $(CFLAGS) $(LFLAGS)

src/display.o: src/display.h src/display.c
	$(CC) -c src/display.c -o src/display.o $(CFLAGS) 

run:
	./render.out

clean: 
	rm render.out
