#include "color.h"

extern color32_t createColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);
extern uint8_t colorAlpha(color32_t color);
extern uint8_t colorRed(color32_t color);
extern uint8_t colorGreen(color32_t color);
extern uint8_t colorBlue(color32_t color);
extern uint8_t blendBits(uint8_t alpha1, uint8_t alpha2, uint8_t c1, uint8_t c2);
extern color32_t blendColors(color32_t color1, color32_t color2);
