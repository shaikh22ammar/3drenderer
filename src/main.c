#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "display.h"
#include "vector.h"

#define CUBE_DIM 10
vec3_t cubePointCloud[CUBE_DIM * CUBE_DIM * CUBE_DIM];
vec3_t cubeStart;
vec2_t projectedPoints[CUBE_DIM * CUBE_DIM * CUBE_DIM];
vec3_t cubeRotation = {.x = 0, .y = 0, .z = 0};
vec3_t cubeCenter;
float cubeStep;

int previousFrameTime = 0;

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
	cubeStart = (vec3_t) {.x = -1, .y = -1, .z = 2};
	vec3_t currentPoint = cubeStart;
	cubeStep = 2.0 / (CUBE_DIM - 1);
	cubeCenter = (vec3_t) {
		.x = (cubeStart.x + cubeStart.x + cubeStep*CUBE_DIM) * 0.5,
		.y = (cubeStart.y + cubeStart.y + cubeStep*CUBE_DIM) * 0.5,
		.z = (cubeStart.z + cubeStart.z + cubeStep*CUBE_DIM) * 0.5
	};
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
		currentPoint.z += cubeStep;
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
	cubeRotation.x += 0.01;
	cubeRotation.y += 0.01;
	cubeRotation.z += 0.01;
	for (int i = 0; i < CUBE_DIM * CUBE_DIM * CUBE_DIM; i++) {
		vec3_t point = cubePointCloud[i];
		point = addVec3(point, scaleVec3(-1.0, cubeCenter));
		point = rotateVec3(point, cubeRotation.x, 'x');
		point = rotateVec3(point, cubeRotation.y, 'y');
		point = rotateVec3(point, cubeRotation.z, 'z');
		point = addVec3(point, cubeCenter);
		projectedPoints[i] = projectPoint(point);
	}

	int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - previousFrameTime);
	if (timeToWait >0 && timeToWait <= FRAME_TARGET_TIME)
		SDL_Delay(timeToWait);
	previousFrameTime = SDL_GetTicks();
}

void render(void) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	
	clearColorBuffer(0xFF000000);
	drawDotGrid(30, 0x00FFFFFF | (65U << 24));
	
	vec3_t pointToDisplay = (vec3_t) {.x = -1, .y = 0, .z = -1};
	vec2_t point = projectPoint(pointToDisplay);
	pixel_t pixel = screenSpaceToPixelSpace(point);
	for (int i = 0; i < CUBE_DIM * CUBE_DIM * CUBE_DIM; i++) {
		pixel_t currentPixel = screenSpaceToPixelSpace(projectedPoints[i]);
		drawRectangle(currentPixel.x - 2, currentPixel.y - 2, 4, 4,  0xFFFFFF00);
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
