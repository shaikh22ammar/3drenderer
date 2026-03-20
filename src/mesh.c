#include "mesh.h"
#include <stdlib.h>
#include "stdio.h"
#include "display.h"
#include "vector.h"
#include <string.h>
#include <stdbool.h>

bool initializeMesh(mesh_t *mesh, int nVertices, int nFaces, vec3_t *vertices, face_t *faces, vec3_t origin) {
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

	mesh->origin = origin;

	// memcpy
	memcpy(mesh->vertices, vertices, sizeof(vec3_t)*nVertices);
	memcpy(mesh->faces, faces, sizeof(face_t)*nFaces);


	return true;
}

void drawMesh(mesh_t *mesh, uint32_t color) {
	vec3_t *vertices = mesh->vertices;
	face_t *faces = mesh->faces;
	vec3_t origin = mesh->origin;
	for (int i = 0; i < mesh->nFaces; i++) {
		face_t face = faces[i];
		vec3_t va = addVec3(origin, vertices[face.a]);
		vec3_t vb = addVec3(origin, vertices[face.b]);
		vec3_t vc = addVec3(origin, vertices[face.c]);
		pixel_t a = screenSpaceToPixelSpace(projectPoint(va));
		pixel_t b = screenSpaceToPixelSpace(projectPoint(vb));
		pixel_t c = screenSpaceToPixelSpace(projectPoint(vc));
		drawTriangle(
			a.x, a.y,
			b.x, b.y,
			c.x, c.y,
			color
		);
	}
}

void drawMeshVertices(mesh_t *mesh, uint32_t color) {
	vec3_t *vertices = mesh->vertices;
	int nVertices = mesh->nVertices;
	vec3_t origin = mesh->origin;
	for (int i = 0; i < nVertices; i++) {
		vec3_t currVertex = vertices[i];
		vec3_t shiftedVertex = addVec3(origin, currVertex);
		vec2_t projectedVertex = projectPoint(shiftedVertex);
		pixel_t rasteredVertex = screenSpaceToPixelSpace(projectedVertex);
		drawRectangle(rasteredVertex - 2, rasteredVertex.y - 2, 4, 4, color);
	}
}

void rotateMesh(mesh_t *mesh, float theta, char axis) {
	int nVertices = mesh->nVertices;
	vec3_t *vertices = mesh->vertices;
	for (int i = 0; i < nVertices; i++) {
		vertices[i] = rotateVec3(vertices[i], theta, axis);
	}
}

void destroyMesh(mesh_t *mesh) {
	free(mesh->vertices);
	free(mesh->faces);
}

