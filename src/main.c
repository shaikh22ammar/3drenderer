#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "texture.h"
#include "color.h"


// mesh details
#define NUM_VERTICES 8
#define NUM_FACES 12
texturedMesh_t *mesh;

int previousFrameTime = 0;

/*bool createCubeMesh(void) {
	 vec3_t vertices[NUM_VERTICES] = {
		(vec3_t) {.x = -0.5, .y = -0.5, .z = 0.5}, // 0
		(vec3_t) {.x = 0.5, .y = -0.5, .z = 0.5},  // 1
		(vec3_t) {.x = 0.5, .y = 0.5, .z = 0.5},   // 2
		(vec3_t) {.x = -0.5, .y = 0.5, .z = 0.5},  // 3
		(vec3_t) {.x = -0.5, .y = -0.5, .z = -0.5}, // 4
		(vec3_t) {.x = 0.5, .y = -0.5, .z = -0.5},  // 5
		(vec3_t) {.x = 0.5, .y = 0.5, .z = -0.5},   // 6
		(vec3_t) {.x = -0.5, .y = 0.5, .z = -0.5},  // 7
	};
	face_t faces[NUM_FACES] = {
		(face_t) {.a = 0, .b = 1, .c = 2}, // back
		(face_t) {.a = 2, .b = 3, .c = 0},
		(face_t) {.a = 4, .b = 7, .c = 6}, // front
		(face_t) {.a = 6, .b = 5, .c = 4},
		(face_t) {.a = 2, .b = 1, .c = 5}, // right
		(face_t) {.a = 5, .b = 6, .c = 2},
		(face_t) {.a = 4, .b = 0, .c = 3}, // left
		(face_t) {.a = 3, .b = 7, .c = 4},
		(face_t) {.a = 0, .b = 4, .c = 5}, // top
		(face_t) {.a = 5, .b = 1, .c = 0},
		(face_t) {.a = 3, .b = 2, .c = 6}, // bottom
		(face_t) {.a = 6, .b = 7, .c = 3},
	};
	vec3_t origin = {.x = 0.0, .y = 0.0, .z = 3.0};

	return initializeMesh(&mesh, NUM_VERTICES, NUM_FACES, vertices, faces, origin);
}*/

bool loadMeshFromAssets() {
	mesh = (texturedMesh_t *) malloc (sizeof(texturedMesh_t));

	int nVertices, nFaces;
	vec3_t *vertices;
	face_t *faces;
	vec2_t *uv;
	bool insideOut = true;
	if (!readWavefront("./assets/cube.obj", &nVertices, &nFaces, &vertices, &faces, &uv, insideOut)) {
		return false;
	}
	if (uv == NULL) {
		return false;
	}
	int texWidth, texHeight;
	uint32_t *texture;
	if(!loadPNG("./assets/cube.png", &texture, &texWidth, &texHeight)) {
		return false;
	}
	vec3_t origin = (vec3_t) {.x = 0, .y = 0, .z = 5};
	if (!initializeMesh(mesh, nVertices, nFaces, vertices, faces, origin, texture, texWidth, texHeight, uv)) {
		return false;
	}
	free(vertices);
	free(faces);
	free(uv);
	vertices=NULL;
	faces=NULL;
	uv=NULL;
	return true;
}

bool setup(void) {
	// Creating raw pixel buffer
	colorBuffer = (color32_t *) malloc(sizeof(color32_t) * windowWidth * windowHeight);
	if (!colorBuffer) {
		free(colorBuffer);
		fprintf(stderr, "Failed to allocate memory for color buffer");
		return false;
	}

	// Creating buffer texture
	colorBufferTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		windowWidth,
		windowHeight
	);
	if (!colorBufferTexture) {
		SDL_Log("Unable to create color buffer texture %s", SDL_GetError());
		return false;
	}

	// Loading objects in scene
	return loadMeshFromAssets();	
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
			else if (event.key.key == SDLK_W)
				RENDER_METHOD.wire ^= 1u;
			else if (event.key.key == SDLK_F)
				RENDER_METHOD.fill ^= 1u;
			else if (event.key.key == SDLK_C)
				RENDER_METHOD.cull ^= 1u;
			else if (event.key.key == SDLK_V)
				RENDER_METHOD.vertex ^= 1u;
			break;
	}
}

void update(void) {
	rotateMesh((mesh_t *) mesh, 0.01, 'x');
	//rotateMesh((mesh_t *) mesh, 0.01, 'y');
	//rotateMesh((mesh_t *) mesh, 0.01, 'z');

	int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - previousFrameTime);
	if (timeToWait >0 && timeToWait <= FRAME_TARGET_TIME)
		SDL_Delay(timeToWait);
	previousFrameTime = SDL_GetTicks();
}

void render(void) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	
	clearColorBuffer(0x00000000);
	drawGrid(100, 0xFFFFFF00|45U);
	drawGrid(500, 0xFFFFFF00|100U);

	drawMesh(&(mesh->mesh), 0x00FF00FF, RENDER_METHOD, 50u, 2);

	renderColorBuffer();
	SDL_RenderPresent(renderer);
}

int main() {
	isRunning = initializeWindow();
	isRunning = setup();
	printf("Window width and height = %d, %d\n", windowWidth, windowHeight);
	while(isRunning) {
		processInput();
		update();	
		render();
	}
	destroyWindow();
	destroyMesh(mesh);

	color32_t white45 = 0xFFFFFF00|45U;
	color32_t black00 = 0U;
	
	color32_t blendedColor = blendColors(black00, white45);

	return 0;
}
