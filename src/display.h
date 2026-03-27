#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "vector.h"
#include "color.h"

#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)

typedef struct {int x; int y;} pixel_t;

extern bool isRunning; 
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern color32_t *colorBuffer;
extern SDL_Texture *colorBufferTexture;
extern int windowWidth, windowHeight;

extern struct renderMethod_t {
	unsigned int wire : 1;
	unsigned int vertex : 1;
	unsigned int cull : 1;
	unsigned int fill : 1;
} RENDER_METHOD;


void updateScreenSpaceCoordinates(void);
vec2_t projectPoint(vec3_t point);
pixel_t screenSpaceToPixelSpace(vec2_t coordinate);

bool initializeWindow(void);
void renderColorBuffer(void);
void clearColorBuffer(color32_t color);
void destroyWindow(void);
void drawPixel(int x, int y, color32_t color); 
void drawGrid(int step, color32_t color);
void drawDotGrid(int step, color32_t color);
void drawRectangle(int x, int y, int width, int height, color32_t color);
void drawLine(int x0, int y0, int x1, int y1, color32_t color);
void drawTriangle(int x0, int y0,
		int x1, int y1,
		int x2, int y2,
		color32_t color);
void fillTriangle(int x0, int y0,
		int x1, int y1,
		int x2, int y2,
		color32_t color);
#endif
