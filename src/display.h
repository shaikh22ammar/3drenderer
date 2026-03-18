#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

extern bool isRunning; 
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *colorBuffer;
extern SDL_Texture *colorBufferTexture;
extern int window_width, window_height;

bool initializeWindow(void);
void renderColorBuffer(void);
void clearColorBuffer(uint32_t color);
void destroyWindow(void);
void drawGrid(int step);

#endif
