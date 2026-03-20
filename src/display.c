#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "math.h"

bool isRunning = false; 
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *colorBuffer = NULL;
SDL_Texture *colorBufferTexture = NULL;
int windowWidth = 800; 
int windowHeight = 600;

// screen space coordinates of left, right, top, and bottom edges of screeen
float screenLeft = -1.0; 
float screenRight = 1.0;
float screenUp; 
float screenDown;

// reciprocal of pixel dimension in screen space coordinates
float oneByPixelDim;

// coordinate of camera in front of the screen
float camZ = -1.0;

void updateScreenSpaceCoordinates(void) {
	/* Updates the coordiantes of top and bottom edges of screen according to window width and height.
	 * It is assumed that screenUp = - screenDown.
	 * We use the equation screenUp + windowHeight * pixelDim = screenUp,
	 * where pixelDim is the width of the pixel that is found using the equation:
	 * screenLeft + windowWidth * pixelDim = screenRight */
	oneByPixelDim = windowWidth * 1.0 / (screenRight - screenLeft);
	screenUp = - 0.5 * windowHeight * (1.0 / oneByPixelDim);
	screenDown = -screenUp;
}

pixel_t screenSpaceToPixelSpace(vec2_t coordinate) {
	/* Maps a coordinate in screen space to the pixel it belongs to using
	 * screenLeft + x * pixelDim <= p.x < screenLeft + (x+1) * pixelDim,
	 * screenDown + y * pixelDim <= p.y < screenUp + (y+1) * pixelDim. */
	if (coordinate.x == NAN || coordinate.y == NAN) {
		return (pixel_t) {.x = -1, .y = -1};
	}
	int x = (int) ((coordinate.x - screenLeft) * oneByPixelDim);
	int y = (int) ((coordinate.y - screenUp) * oneByPixelDim);
	return (pixel_t) {.x = x, .y = y};
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
	/* Projects a point using the fact that
	 * |point.x|/|x'| = |point.z - camZ|/|camZ|
	 * |point.y|/|y'| = |point.z - camZ|/|camZ| */
	if (point.z < 0) {
		return (vec2_t) {.x = NAN, .y = NAN};
	}
	float x = point.x * camZ / (-point.z + camZ);
	float y = point.y * camZ / (-point.z + camZ);
	return (vec2_t) {.x = x, .y = y};
}

void drawPixel(int x, int y, uint32_t color) {
	/* Fills the pixel at the y-th row and x-th column with color.
	 * Note that y goes down as it increases.
	 */
	if (y >=0 && y < windowHeight && x >=0 && x < windowWidth) {
		colorBuffer[y*windowWidth + x] = color;
	}
}

void drawGrid(int step, uint32_t color) {
	// draw vertical lines
	for (int j = step - 1; j < windowWidth; j+= step) {
		for (int i = 0; i < windowHeight; i++) {
			drawPixel(j, i, color);
		}
	}
	// draw horizontal lines
	for (int i = step - 1; i < windowHeight; i+= step) {
		for (int j = 0; j < windowWidth; j++) {
			drawPixel(j, i, color);
		}
	}
}

void drawDotGrid(int step, uint32_t color) {
	for (int i = step - 1; i < windowHeight; i += step) {
		for (int j = step - 1; j < windowWidth; j += step) {
			drawPixel(j, i, color);
		}
	}
}

void drawRectangle(int x, int y, int width, int height, uint32_t color) {
	for (int i = y; i < y + height; i++) {
		for (int j = x; j < x + width; j++) {
			drawPixel(j, i, color);
		}
	}
}

void drawLine(int x0, int y0, int x1, int y1, uint32_t color) {
	/* The algorithm draws a line from the (x0, y0)-th pixel to (x1, y1)-th pixel.
	 * dy = y1 - y0, dx = x1 - x0, m = dy/dx.
	 * x0 and y0 can be interpreted as the abcissa and ordinate of the center of the (x0, y0)-th pixel.
	 * The "true" ordinate of x-th pixel is f(x) = m * (x - x0) + y0, 
	 * the actual "ordinate" is round(f(x)) since coordinates of center of pixels must be integral.
	 * The error epsilon if f(x) - round(f(x)).
	 * After finding the actual ordinate of x0 + 1, the error is recorded and x0 is incremented, 
	 * then the same steps are repeated until x0 equals x1.
	 *
	 * First, assume 0 <= m <= 1. In this case, there is exactly one pixel lighting up for every column between x0 and x1.
	 * Given x0, and its actual ordinate y0, and the error epsilon, this is how the ordinate of x0 + 1 is found:
	 * The true ordinate of x0 is y0 + epsilon, 
	 * the true ordinate of x0 + 1 is hence, y0 + epsilon + m.
	 * Therefore, the actual ordinate of x0 + 1 is y0 + 1 iff
	 * 	y0 + epsilon + m - y0 - 1 < -1/2,
	 * in which case, the error is updated to epsilon + m - 1, otherwise 2 - epsilon - m
	 *
	 * In the case -1 <= m < 0, we decrease y instead of increasing
	 *
	 * The cases, |m| > 1 is handle by iterating from y0 to y1 instead of x0 to x1
	 *
	 * To avoid floating point arithmetic, denominators are normalized, and we instead record epsilon * dx
	 * This is intialized first as 0 since the initial (y0, x0) has no error.
	 */

	

	int dy = y1 - y0;
	int dx = x1 - x0;

	// case when |m| <= 1 
	if (abs(dy) <= abs(dx)) {
		if (dx < 0) {
			x0 = x0^x1;
			x1 = x0^x1;
			x0 = x0^x1;
			dx = -dx;

			y0 = y0^y1;
			y1 = y0^y1;
			y0 = y0^y1;
			dy = -dy;
		}
		int signDy = (dy > 0) - (dy < 0);
		int epsDx = 0;
		while (x0 <= x1) {
			drawPixel(x0, y0, color);
			if (abs(2*epsDx + 2*dy - 2*signDy*dx) <= abs(dx)) {
				epsDx += dy - signDy * dx;
				y0 += signDy;
			} else {
				epsDx += dy;
			}
			x0++;
		}
		return;
	}
	// case when |m| > 1
	if (dy < 0) {
		x0 = x0^x1;
		x1 = x0^x1;
		x0 = x0^x1;
		dx = -dx;

		y0 = y0^y1;
		y1 = y0^y1;
		y0 = y0^y1;
		dy = -dy;
	}
	while (y0 <= y1) {
		int signDx = (dx > 0) - (dx < 0);
		int epsDy = 0;
		while (y0 <= y1) {
			drawPixel(x0, y0, color);
			if (abs(2*epsDy + 2*dx - 2*signDx*dy) <= abs(dy)) {
				epsDy += dx - signDx*dy;
				x0 += signDx;
			} else {
				epsDy += dx;
			}
			y0++;
		}
	}
}
