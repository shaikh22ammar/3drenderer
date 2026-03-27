#ifndef MESH_H
#define MESH_H

#include "display.h"
#include "vector.h"

typedef struct {
	int a;
	int b;
	int c;
	int ua;
	int ub;
	int uc;
} face_t;

typedef struct {
	int nVertices;
	int nFaces;
	vec3_t *vertices;
	face_t *faces;	
	vec3_t origin;
} mesh_t;

typedef struct {
	mesh_t mesh;
	uint32_t *texture;
	int texWidth;
	int texHeight;
	vec2_t *uv;
} texturedMesh_t;

bool initializeMesh(
		texturedMesh_t *mesh, 
		int nVertices, int nFaces, 
		vec3_t *vertices, face_t *faces, 
		vec3_t origin,
		uint32_t *texture, int texWidth, int texHeight, vec2_t *uv 
		); 
void drawMesh(const mesh_t *mesh, const uint32_t color, const struct renderMethod_t, uint8_t fillOpacity, const int vertexRadius);
void destroyMesh(texturedMesh_t *mesh);
void rotateMesh(mesh_t *mesh, float theta, char axis);
bool readWavefront(char *filepath, int *nVertices, int *nFaces, vec3_t **vertices, face_t **faces, vec2_t **uv, const bool insideOut);

#endif
