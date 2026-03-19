#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "display.h"

#define CUBE_DIM 10
vec3_t cubePointCloud[CUBE_DIM * CUBE_DIM * CUBE_DIM];
vec3_t cubeStart;
vec2_t projectedPoints[CUBE_DIM * CUBE_DIM * CUBE_DIM];

bool setup(void) {
	// Creating raw pixel buffer
	colorBuffer = (uint32_t *) malloc(sizeof(uint32_t) * windowWidth * windowHeight);
	if (!colorBuffer) {
		fprintf(stderr, "Failed to allocate memory for color buffer");
		return false;
	}

	// Creating buffer texture
	colorBufferTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		windowWidth,
		windowHeight
	);
	if (!colorBufferTexture) {
		SDL_Log("Unable to create color buffer texture %s", SDL_GetError());
		return false;
	}

	// Creating cube point cloud
	cubeStart = (vec3_t) {.x = -1, .y = -1, .z = -2};
	vec3_t currentPoint = cubeStart;
	float cubeStep = 2.0 / (CUBE_DIM - 1);
	int  zCount = 0;
	while (zCount < CUBE_DIM) {
		int yCount = 0;
		currentPoint.y = cubeStart.y;
		while (yCount < CUBE_DIM) {
			int xCount = 0;
			currentPoint.x = cubeStart.x;
			while(xCount < CUBE_DIM) {
				cubePointCloud[zCount*CUBE_DIM*CUBE_DIM + yCount*CUBE_DIM + xCount] = currentPoint;
				currentPoint.x += cubeStep;
				xCount++;

			}
			currentPoint.y += cubeStep;
			yCount++;
		}
		currentPoint.z -= cubeStep;
		zCount++;
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

void update(void) {
	for (int i = 0; i < CUBE_DIM * CUBE_DIM * CUBE_DIM; i++) {
		projectedPoints[i] = projectPoint(cubePointCloud[i]);
	}
}

void render(void) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	
	clearColorBuffer(0xFF000000);
	drawDotGrid(30, 0x00FFFFFF | (65U << 24));
	
	//vec3_t pointToDisplay = (vec3_t) {.x = -1, .y = 0, .z = -1};
	//vec2_t point = projectPoint(pointToDisplay);
	//pixel_t pixel = screenSpaceToPixelSpace(point);
	//drawPixel(pixel.i, pixel.j, 0xFFFFFFFF);
	for (int i = 0; i < CUBE_DIM * CUBE_DIM * CUBE_DIM; i++) {
		pixel_t currentPixel = screenSpaceToPixelSpace(projectedPoints[i]);
		drawRectangle(currentPixel.i, currentPixel.j, 4, 4,  0xFFFFFF00);
	}
	
	renderColorBuffer();

	SDL_RenderPresent(renderer);
}

int main() {
	isRunning = initializeWindow();
	isRunning = setup();
	while(isRunning) {
		processInput();
		update();	
		render();
	}
	destroyWindow();
	return 0;
}
