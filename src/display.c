#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

bool isRunning = false; 
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *colorBuffer = NULL;
SDL_Texture *colorBufferTexture = NULL;
int windowWidth = 800; 
int windowHeight = 600;

float screenLeft = -1;
float screenRight = 1;
float screenUp;
float screenDown;

float camZ = 1;

void updateScreenSpaceCoordinates(void) {
	screenUp = 0.5 * windowHeight / windowWidth;
	screenDown = -screenUp;
}

pixel_t screenSpaceToPixelSpace(vec2_t coordinate) {
	int x = (int) (coordinate.x - screenLeft) * 2.0 / windowWidth;
	int y = (int) (coordinate.y - screenDown) * 2.0 / windowWidth;
	return (pixel_t) {.i = x, .j = y};
}

bool initializeWindow(void) {
	// Intialization
	SDL_InitFlags initFlags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
	if (!SDL_Init(initFlags)) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}

	// Creating window with properties
	const SDL_DisplayMode *display_mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
	windowWidth = display_mode->w;
	windowHeight = display_mode->h;
	updateScreenSpaceCoordinates();
	SDL_PropertiesID props = SDL_CreateProperties();
	if (props == 0) {
		SDL_Log("Unable to create properties: %s", SDL_GetError());
		return false;
	}
	SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, NULL);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, windowWidth);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, windowHeight);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
	SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_FULLSCREEN_BOOLEAN, true);

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

void renderColorBuffer(void) {
	SDL_UpdateTexture(
		colorBufferTexture,
		NULL,
		colorBuffer,
		(int) (windowWidth * sizeof (uint32_t))
	);
	SDL_RenderTexture(renderer, colorBufferTexture, NULL, NULL);
}

void clearColorBuffer(uint32_t color) {
	for (int i = 0; i < windowHeight; i++) {
		for (int j = 0; j < windowWidth; j++) {
			colorBuffer[(i*windowWidth) + j] = color;
		}
	}
}

void destroyWindow(void) {
	SDL_DestroyTexture(colorBufferTexture);
	free(colorBuffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

vec2_t projectPoint(vec3_t point) {
	float x = point.x * camZ / (point.z - camZ);
	float y = point.y * camZ / (point.z - camZ);
	return (vec2_t) {.x = x, .y = y};
}

void drawPixel(int i, int j, uint32_t color) {
	if (i >=0 && i < windowHeight && j >=0 && j < windowWidth) {
		colorBuffer[i*windowWidth + j] = color;
	}
}

void drawGrid(int step, uint32_t color) {
	// draw vertical lines
	for (int j = step - 1; j < windowWidth; j+= step) {
		for (int i = 0; i < windowHeight; i++) {
			drawPixel(i, j, color);
		}
	}
	// draw horizontal lines
	for (int i = step - 1; i < windowHeight; i+= step) {
		for (int j = 0; j < windowWidth; j++) {
			drawPixel(i, j, color);
		}
	}
}

void drawDotGrid(int step, uint32_t color) {
	for (int i = step - 1; i < windowHeight; i += step) {
		for (int j = step - 1; j < windowWidth; j += step) {
			drawPixel(i, j, color);
		}
	}
}

void drawRectangle(int i, int j, int width, int height, uint32_t color) {
	for (int y = i; y < i + height; y++) {
		for (int x = j; x < j + width; x++) {
			drawPixel(i, j, color);
		}
	}
}
