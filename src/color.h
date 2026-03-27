#ifndef COLOR_H
#define COLOR_H
#include <stdint.h>
typedef uint32_t color32_t;

inline color32_t createColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
	return ((color32_t)alpha) | ((color32_t)blue << 8) | ((color32_t)green << 16) | ((color32_t)red << 24);
}

inline uint8_t colorAlpha(color32_t color) {
	uint32_t mask = 0x000000FF;
	color &= mask;
	uint8_t alpha = (uint8_t) (color);
	return alpha;
}

inline uint8_t colorRed(color32_t color) {
	uint32_t mask = 0xFF000000;
	color &= mask;
	uint8_t red = (uint8_t) (color >> 24);
	return red;
}

inline uint8_t colorGreen(color32_t color) {
	uint32_t mask = 0x00FF0000;
	color &= mask;
	uint8_t green = (uint8_t) (color >> 16);
	return green;
}

inline uint8_t colorBlue(color32_t color) {
	uint32_t mask = 0x0000FF00;
	color &= mask;
	uint8_t blue = (uint8_t) (color >> 8);
	return blue;
}

inline uint8_t blendBits(uint8_t alpha1, uint8_t alpha2, uint8_t c1, uint8_t c2) {
	return (
		(255*alpha2*c2 + (255 - alpha2)*alpha1*c1) / (255*alpha1 + 255*alpha2 - alpha1*alpha2)
	);
}

inline color32_t blendColors(color32_t color1, color32_t color2) {
	uint8_t alpha1 = colorAlpha(color1);
	uint8_t alpha2 = colorAlpha(color2);
	uint8_t alpha = (255*alpha1 + 255*alpha2 - (alpha1 * alpha2)) / 255;

	uint8_t red1 = colorRed(color1);
	uint8_t red2 = colorRed(color2);
	uint8_t red = blendBits(alpha1, alpha2, red1, red2);

	uint8_t green1 = colorGreen(color1);
	uint8_t green2 = colorGreen(color2);
	uint8_t green = blendBits(alpha1, alpha2, green1, green2);

	uint8_t blue1 = colorBlue(color1);
	uint8_t blue2 = colorBlue(color2);
	uint8_t blue = blendBits(alpha1, alpha2, blue1, blue2);

	return createColor(red, green, blue, alpha);
}
#endif
