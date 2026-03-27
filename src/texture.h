#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

bool loadPNG(char *filepath, uint32_t **buffer, int *width, int *height); 

#endif
