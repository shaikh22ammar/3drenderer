#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

bool isRunning = false;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *colorBuffer = NULL;
SDL_Texture *colorBufferTexture = NULL;
int window_width = 800, window_height = 600;

bool intialize_window(void) {
	// Intialization
	SDL_InitFlags initFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
	if (!SDL_Init(initFlags)) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Creating window with properties
	SDL_PropertiesID props = SDL_CreateProperties();
	if (props == 0) {
		SDL_Log("Unable to create properties: %s", SDL_GetError());
		return false;
	}
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, NULL);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, window_width);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, window_height);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, true);

	window = SDL_CreateWindowWithProperties(props);
	if (window == NULL) {
		SDL_Log("Unable to create window: %s", SDL_GetError());
		return false;
	}

	// Creating renderer
	renderer = SDL_CreateRenderer(window, NULL);
	if (renderer == NULL) {
		SDL_Log("Unable to create renderer: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool setup(void) {
	colorBuffer = (uint32_t *) malloc(sizeof(uint32_t) * window_width * window_height);
	if (!colorBuffer) {
		fprintf(stderr, "Failed to allocate memory for color buffer");
		return false;
	}
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

void renderColorBuffer(void) {
	SDL_UpdateTexture(
		colorBufferTexture,
		NULL,
		colorBuffer,
		(int) (window_width * sizeof (uint32_t))
	);
	SDL_RenderTexture(renderer, colorBufferTexture, NULL, NULL);
}

void clearColorBuffer(uint32_t color) {
	for (int i = 0; i < window_height; i++) {
		for (int j = 0; j < window_width; j++) {
			colorBuffer[(i*window_width) + j] = color;
		}
	}
}

void render(void) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	renderColorBuffer();
	clearColorBuffer(0xFFFFFF00);

	SDL_RenderPresent(renderer);
}

void destroyWindow(void) {
	free(colorBuffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main() {
	isRunning = intialize_window() && setup();
	while(isRunning) {
		processInput();
		//clearColorBuffer(0xFFFFFF00);
		render();
	}
	destroyWindow();
	return 0;
}
