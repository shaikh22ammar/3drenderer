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

void drawMesh(const mesh_t *mesh, const uint32_t color, const struct renderMethod_t RENDER_METHOD, uint8_t fillOpacity, const int vertexRadius) {
	vec3_t *vertices = mesh->vertices;
	face_t *faces = mesh->faces;
	vec3_t origin = mesh->origin;

	for (int i = 0; i < mesh->nFaces; i++) {
		face_t face = faces[i];
		vec2_t va = projectPoint(addVec3(origin, vertices[face.a]));
		vec2_t vb = projectPoint(addVec3(origin, vertices[face.b]));
		vec2_t vc = projectPoint(addVec3(origin, vertices[face.c]));

		if (isnan(va.x) || isnan(vb.x) || isnan(vc.x)) continue;

		pixel_t a = screenSpaceToPixelSpace(va);
		pixel_t b = screenSpaceToPixelSpace(vb);
		pixel_t c = screenSpaceToPixelSpace(vc);

		if (RENDER_METHOD.cull) {
			if (
			b.x*c.y - c.x*b.y 
			- a.x*c.y + c.x*a.y
			+ a.x*b.y - b.x*a.y > 0) {
				continue;
			}
		}
		if (RENDER_METHOD.fill) {
			fillTriangle(
				a.x, a.y,
				b.x, b.y,
				c.x, c.y,
				(color & 0x00FFFFFF) | ((uint32_t) fillOpacity<<24)
			);
		} 
		if (RENDER_METHOD.wire) {
			drawTriangle(
				a.x, a.y,
				b.x, b.y,
				c.x, c.y,
				color
			);
		}
		if (RENDER_METHOD.vertex) {
			drawRectangle(a.x - vertexRadius, a.y - vertexRadius, 2*vertexRadius, 2*vertexRadius, color);
			drawRectangle(b.x - vertexRadius, b.y - vertexRadius, 2*vertexRadius, 2*vertexRadius, color);
			drawRectangle(c.x - vertexRadius, c.y - vertexRadius, 2*vertexRadius, 2*vertexRadius, color);
		}
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

bool readWavefront(char *filepath, int *nVertices, int *nFaces, vec3_t **vertices, face_t **faces, const bool insideOut) {
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
				v.z = -1.0f*v.z;
				v.y = -1.0f*v.y;
				verts[vCount] = v;
				vCount++;
			}
		} else if (strncmp(line, "f ", 2) == 0) {
			char ta[64], tb[64], tc[64];
			if (sscanf(line, "f %s %s %s", ta, tb, tc) == 3) {
				if (insideOut)
					facs[fCount] = (face_t) {.c = atoi(ta) - 1, .b = atoi(tb) - 1, .a = atoi(tc) - 1};
				else
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
