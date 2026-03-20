#include "mesh.h"
#include <stdlib.h>
#include "stdio.h"
#include "display.h"
#include "vector.h"
#include <string.h>
#include <stdbool.h>

bool initializeMesh(mesh_t *mesh, int nVertices, int nFaces, vec3_t *vertices, face_t *faces) {
	mesh->nVertices = nVertices;
	mesh->nFaces = nFaces;

	// mallocate 
	mesh->vertices = (vec3_t *) malloc(sizeof(vec3_t)*nVertices);
	mesh->faces = (face_t *) malloc(sizeof(face_t)*nFaces);

	if(!(mesh->vertices && mesh->faces)) {
		free(mesh->vertices);
		free(mesh->faces);
		return false;
	}

	// memcpy
	memcpy(mesh->vertices, vertices, sizeof(vec3_t)*nVertices);
	memcpy(mesh->faces, faces, sizeof(face_t)*nFaces);

	float oneByEnVertices = 1.0 / nVertices;
	vec3_t centroid = scaleVec3(oneByEnVertices, vertices[0]);
	for (int i = 1; i < nVertices; i++) {
		centroid = addVec3(centroid, scaleVec3(oneByEnVertices, vertices[i]));
	}
	mesh->centroid = centroid;

	return true;
}

void drawMesh(mesh_t *mesh, uint32_t color) {
	vec3_t *vertices = mesh->vertices;
	face_t *faces = mesh->faces;
	for (int i = 0; i < mesh->nFaces; i++) {
		face_t face = faces[i];
		pixel_t a = screenSpaceToPixelSpace(projectPoint(vertices[face.a]));
		pixel_t b = screenSpaceToPixelSpace(projectPoint(vertices[face.b]));
		pixel_t c = screenSpaceToPixelSpace(projectPoint(vertices[face.c]));
		drawTriangle(
			a.x, a.y,
			b.x, b.y,
			c.x, c.y,
			color
		);
	}
}

void rotateMesh(mesh_t *mesh, float theta, char axis) {
	int nVertices = mesh->nVertices;
	vec3_t *vertices = mesh->vertices;
	vec3_t centroid = mesh->centroid;
	vec3_t negCentroid = scaleVec3(-1.0, centroid);
	for (int i = 0; i < nVertices; i++) {
		vec3_t currVertex = vertices[i];
		currVertex = addVec3(vertices[i], negCentroid);
		currVertex = rotateVec3(currVertex, theta, axis);
		vertices[i] = addVec3(currVertex, centroid);
	}
}

void destroyMesh(mesh_t *mesh) {
	free(mesh->vertices);
	free(mesh->faces);
}

