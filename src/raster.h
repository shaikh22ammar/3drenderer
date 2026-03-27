#ifndef RASTER_H
#define RASTER_H

#include "display.h"

inline int area2(pixel_t a, pixel_t b, pixel_t c) {
	/*
	 * Computes the determinant
	 * | 1 a.x a.y |
	 * | 1 b.x b.y |
	 * | 1 c.x c.y |
	 *
	 * */
	return (b.x*c.y - c.x*b.y) - (a.x*c.y - c.x*a.y) + (a.x*b.y - b.x*a.y);
} 

#endif
