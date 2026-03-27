#include "upng.h"
#include <stdio.h>
#include "texture.h"


static upng_t *upng = NULL;

bool loadPNG(char *filepath, uint32_t **buffer, int *width, int *height) {
	upng = upng_new_from_file(filepath);
	if (upng == NULL) {
		fprintf(stderr, "Loading png failed, upng exited with error %d", upng_get_error(upng));
		goto cleanup;
	}
	upng_decode(upng);
	if (upng_get_error(upng) != UPNG_EOK) {
		fprintf(stderr, "Decoding upng failed, upng exited with error %d", upng_get_error(upng));
		goto cleanup;
	}
	if (upng_get_format(upng) != UPNG_RGBA8) {
		fprintf(stderr, "PNG file is not 32 bit, %d bit instead", upng_get_format(upng));
		goto cleanup;
	}

	*width = upng_get_width(upng);
	*height = upng_get_height(upng);
	*buffer = (uint32_t *) upng_get_buffer(upng);

	upng_free(upng);
	return true;

	cleanup:
		upng_free(upng);
		return false;
}
