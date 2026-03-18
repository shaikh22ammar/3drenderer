#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "display.h"

bool isRunning = false; 
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *colorBuffer = NULL;
SDL_Texture *colorBufferTexture = NULL;
int window_width = 800; 
int window_height = 600;

bool setup(void) {
	// Creating raw pixel buffer
	colorBuffer = (uint32_t *) malloc(sizeof(uint32_t) * window_width * window_height);
	if (!colorBuffer) {
		fprintf(stderr, "Failed to allocate memory for color buffer");
		return false;
	}

	// Creating buffer texture
	colorBufferTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
	if (!colorBufferTexture) {
		SDL_Log("Unable to create color buffer texture %s", SDL_GetError());
		return false;
	}
	return true;
}

void processInput(void) {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch(event.type) {
		case SDL_EVENT_QUIT:
			isRunning = false;
			break;
		case SDL_EVENT_KEY_DOWN:
			if (event.key.key == SDLK_ESCAPE)
				isRunning = false;
			break;
	}
}

void render(void) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	drawGrid(100);
	renderColorBuffer();
	clearColorBuffer(0xFF000000);

	SDL_RenderPresent(renderer);
}

int main() {
	isRunning = initializeWindow() && setup();
	while(isRunning) {
		processInput();
		//clearColorBuffer(0xFFFFFF00);
		render();
	}
	destroyWindow();
	return 0;
}
