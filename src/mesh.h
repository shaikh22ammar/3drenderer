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
	vec3_t centroid;
} mesh_t;

void intializeMesh(mesh_t *mesh, int nVertices, int nFaces, vec3_t *vertices, face_t *faces); 
void drawMesh(mesh_t *mesh, uint32_t color);
void destroyMesh(mesh_t *mesh);
void rotateMesh(mesh_t *mesh, float theta, char axis);

#endif
