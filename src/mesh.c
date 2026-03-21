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
	mesh->origin = origin;

	// mallocate 
	mesh->vertices = (vec3_t *) malloc(sizeof(vec3_t)*nVertices);
	mesh->faces = (face_t *) malloc(sizeof(face_t)*nFaces);

	if(!(mesh->vertices && mesh->faces)) {
		free(mesh->vertices);
		free(mesh->faces);
		mesh->vertices = NULL;
		mesh->faces = NULL;
		fprintf(stderr, "Failed to allocate memory for vertices and faces");
		return false;
	}


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
		drawRectangle(rasteredVertex.x - 2, rasteredVertex.y - 2, 4, 4, color);
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

bool readWavefront(char *filepath, int *nVertices, int *nFaces, vec3_t **vertices, face_t **faces) {
	FILE *file = fopen(filepath, "r");
	if (!file) {
		fprintf(stderr, "Could not open .obj file");
		return false;
	}

	// FIRST PASS: reading the number of vertices and faces
	int vCount = 0, fCount = 0;
	char line[256];
	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "v ", 2) == 0) vCount++;
		if (strncmp(line, "f ", 2) == 0) fCount++;
	}
	*nVertices = vCount;
	*nFaces = fCount;
	
	// Allocating memory for array of vertices and faces
	vec3_t *verts = (vec3_t *) malloc(sizeof(vec3_t)*vCount);
	face_t *facs = (face_t *) malloc(sizeof(face_t)*fCount);
	if (!verts || !facs) {
		fprintf(stderr, "Error allocating memory for vertices and faces");
		fclose(file);
		return false;
	}

	// SECOND PASS: reading vertices and faces
	rewind(file);
	vCount = fCount = 0;
	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "v ", 2) == 0) {	
			vec3_t v;
			if (sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z) == 3) {
				v.z = -1.0*v.z;
				v.y = -1.0*v.y;
				verts[vCount] = v;
				vCount++;
			}
		} else if (strncmp(line, "f ", 2) == 0) {
			char ta[64], tb[64], tc[64];
			if (sscanf(line, "f %s %s %s", ta, tb, tc) == 3) {
				facs[fCount] = (face_t) {.a = atoi(ta) - 1, .b = atoi(tb) - 1, .c = atoi(tc) - 1};
				fCount++;
			}
		}
	}
	*vertices = verts;
	*faces = facs;

	if (vCount != *nVertices || fCount != *nFaces) {
		fprintf(stderr, "Error: Number of vertices/faces read in the first and second pass do not match");
		fclose(file);
		return false;
	} 

	return true;

}
