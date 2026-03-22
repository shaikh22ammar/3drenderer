#ifndef MESH_H
#define MESH_H

#include "display.h"
#include "vector.h"

typedef struct {
	int a;
	int b;
	int c;
} face_t;

typedef struct {
	int nVertices;
	int nFaces;
	vec3_t *vertices;
	face_t *faces;	
	vec3_t origin;
} mesh_t;

bool initializeMesh(mesh_t *mesh, int nVertices, int nFaces, vec3_t *vertices, face_t *faces, vec3_t origin); 
void drawMesh(const mesh_t *mesh, const uint32_t color, const struct renderMethod_t, uint8_t fillOpacity, const int vertexRadius);
void destroyMesh(mesh_t *mesh);
void rotateMesh(mesh_t *mesh, float theta, char axis);
bool readWavefront(char *filepath, int *nVertices, int *nFaces, vec3_t **vertices, face_t **faces, const bool insideOut);

#endif
