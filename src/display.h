#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "vector.h"

typedef struct {int i; int j;} pixel_t;

extern bool isRunning; 
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *colorBuffer;
extern SDL_Texture *colorBufferTexture;
extern int windowWidth, windowHeight;

extern float screenLeft;
extern float screenRight;
extern float screenUp;
extern float screenDown;

extern float camZ;

void updateScreenSpaceCoordinates(void);
vec2_t projectPoint(vec3_t point);
pixel_t screenSpaceToPixelSpace(vec2_t coordinate);

bool initializeWindow(void);
void renderColorBuffer(void);
void clearColorBuffer(uint32_t color);
void destroyWindow(void);
void drawPixel(int i, int j, uint32_t color); 
void drawGrid(int step, uint32_t color);
void drawDotGrid(int step, uint32_t color);
void drawRectangle(int i, int j, int width, int height, uint32_t color);

#endif
