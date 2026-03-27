#include "mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include "stdio.h"
#include "display.h"
#include "vector.h"
#include <string.h>
#include <stdbool.h>

bool initializeMesh(
		texturedMesh_t *mesh, 
		int nVertices, int nFaces, 
		vec3_t *vertices, face_t *faces, 
		vec3_t origin,
		uint32_t *texture, int texWidth, int texHeight, vec2_t *uv 
		) {
	mesh->mesh.nVertices = nVertices;
	mesh->mesh.nFaces = nFaces;
	mesh->mesh.origin = origin;
	mesh->texWidth = texWidth;
	mesh->texHeight = texHeight;

	// mallocate 
	mesh->mesh.vertices = (vec3_t *) malloc(sizeof(vec3_t)*nVertices);
	mesh->mesh.faces = (face_t *) malloc(sizeof(face_t)*nFaces);
	mesh->texture = (uint32_t *) malloc(sizeof(uint32_t)*texWidth*texHeight);
	mesh->uv = (vec2_t *) malloc(sizeof(vec2_t)*nVertices);

	if(!(mesh->mesh.vertices && mesh->mesh.faces)) {
		free(mesh->mesh.vertices);
		free(mesh->mesh.faces);
		mesh->mesh.vertices = NULL;
		mesh->mesh.faces = NULL;
		fprintf(stderr, "Failed to allocate memory for vertices and faces");
		return false;
	}
	if(!(mesh->texture && mesh->uv)) {
		free(mesh->texture);
		mesh->texture = NULL;
		free(mesh->uv);
		mesh->uv = NULL;
		fprintf(stderr, "Failed to allocate memory for texture and uv");
		return false;
	}


	// memcpy
	memcpy(mesh->mesh.vertices, vertices, sizeof(vec3_t)*nVertices);
	memcpy(mesh->mesh.faces, faces, sizeof(face_t)*nFaces);
	memcpy(mesh->texture, texture, sizeof(uint32_t)*texWidth*texHeight);
	memcpy(mesh->uv, uv, sizeof(vec2_t)*nVertices);

	return true;
}

void destroyMesh(texturedMesh_t *mesh) {
	free(mesh->mesh.vertices);
	free(mesh->mesh.faces);
	free(mesh->texture);
	mesh->mesh.vertices = NULL;
	mesh->mesh.faces = NULL;
	mesh->texture = NULL;
	free(mesh);
	mesh = NULL;
}

bool readWavefront(char *filepath, int *nVertices, int *nFaces, vec3_t **vertices, face_t **faces, vec2_t **uv, const bool insideOut) {
	FILE *file = fopen(filepath, "r");
	if (!file) {
		fprintf(stderr, "Could not open .obj file");
		return false;
	}

	// FIRST PASS: reading the number of vertices and faces
	int vCount = 0, fCount = 0, vtCount = 0;
	char line[256];
	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "v ", 2) == 0) vCount++;
		if (strncmp(line, "f ", 2) == 0) fCount++;
		if (strncmp(line, "vt ", 3) == 0) vtCount++;
	}
	*nVertices = vCount;
	*nFaces = fCount;
	int nvtCount = vtCount;
	
	// Allocating memory for array of vertices and faces
	vec3_t *verts = (vec3_t *) malloc(sizeof(vec3_t)*vCount);
	face_t *facs = (face_t *) malloc(sizeof(face_t)*fCount);
	vec2_t *uvs = (vec2_t *) malloc(sizeof(vec2_t)*vtCount);
	if (!verts || !facs || !uvs) {
		fprintf(stderr, "Error allocating memory for vertices, faces, and uvs");
		goto cleanup;
	}

	// SECOND PASS: reading vertices and faces
	rewind(file);
	vCount = fCount = vtCount = 0;
	while (fgets(line, sizeof(line), file)) {
		if (strncmp(line, "v ", 2) == 0) {	
			vec3_t v;
			if (sscanf(line, "v %f %f %f", &v.x, &v.y, &v.z) == 3) {
				v.z = -1.0f*v.z;
				v.y = -1.0f*v.y;
				verts[vCount] = v;
				vCount++;
			}
		} else if (strncmp(line, "vt ", 3) == 0) {
			vec2_t u;
			if (sscanf(line, "vt %f %f", &u.x, &u.y) == 2) {
				if(vtCount >= *nVertices) {
					fprintf(stderr, "Number of texture coordinates exceeds number of vertices");
					goto cleanup;
				}
				u.y = -1.0f*u.y;
				uvs[vtCount] = u;
				vtCount++;
			}
		} else if (strncmp(line, "f ", 2) == 0) {
			int a, b, c, ta, tb, tc, na, nb, nc;
			if (sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
						&a, &ta, &na, 
						&b, &tb, &nb, 
						&c, &tc, &nc) == 9) {
				if (insideOut) {
					facs[fCount] = (face_t) {
						.a = c - 1,
						.ua = tc - 1,
						.b = b - 1,
						.ub = tb - 1,
						.c = a - 1,
						.uc = ta - 1
					};
				} else {
					facs[fCount] = (face_t) {
						.a = a - 1,
						.ua = ta - 1,
						.b = b - 1,
						.ub = tb - 1,
						.c = c - 1,
						.uc = tc - 1
					};
				}
				fCount++;
			}
			
		}
	}

	if (vCount != *nVertices) {
		fprintf(stderr, "Error: Number of vertices read in first and second pass do not match"
				"vCount = %d, but nVertices = %d\n",
				vCount, *nVertices);
		goto cleanup;
	}

	if (fCount != *nFaces) {
		fprintf(stderr, "Error: Number of faces read in first and second pass do not match"
				"fCount = %d, but nFaces = %d\n",
				fCount, *nFaces);
		goto cleanup;
	}

	if (vtCount != nvtCount) {
		fprintf(stderr, "Error: Number of vertices read in first and second pass do not match"
				"vtCount = %d, but nvtCount = %d\n",
				vtCount, nvtCount);
		goto cleanup;
	}

	*vertices = verts;
	*faces = facs;
	*uv = uvs;

	return true;
	
	cleanup:
		fclose(file);
		free(facs);
		free(verts);
		free(uvs);
		facs = NULL;
		verts = NULL;
		uvs = NULL;
		return false;

}

void drawMesh(const mesh_t *mesh, const uint32_t color, const struct renderMethod_t RENDER_METHOD, uint8_t fillOpacity, const int vertexRadius) {
	vec3_t *vertices = mesh->vertices;
	face_t *faces = mesh->faces;
	vec3_t origin = mesh->origin;

	const bool cull = RENDER_METHOD.cull;
	const bool fill = RENDER_METHOD.fill;
	const bool wire = RENDER_METHOD.wire;
	const bool vertex = RENDER_METHOD.vertex;

	color32_t fillColor = (color & 0xFFFFFF00) | ((uint32_t) fillOpacity);

	for (int i = 0; i < mesh->nFaces; i++) {
		face_t face = faces[i];
		vec2_t va = projectPoint(addVec3(origin, vertices[face.a]));
		vec2_t vb = projectPoint(addVec3(origin, vertices[face.b]));
		vec2_t vc = projectPoint(addVec3(origin, vertices[face.c]));

		if (isnan(va.x) || isnan(vb.x) || isnan(vc.x)) continue;

		pixel_t a = screenSpaceToPixelSpace(va);
		pixel_t b = screenSpaceToPixelSpace(vb);
		pixel_t c = screenSpaceToPixelSpace(vc);

		if (cull) {
			if (
			b.x*c.y - c.x*b.y 
			- a.x*c.y + c.x*a.y
			+ a.x*b.y - b.x*a.y < 0) {
				continue;
			}
		}
		if (fill) {
			fillTriangle(
				a.x, a.y,
				b.x, b.y,
				c.x, c.y,
				fillColor
			);
		} 
		if (wire) {
			drawTriangle(
				a.x, a.y,
				b.x, b.y,
				c.x, c.y,
				color
			);
		}
		if (vertex) {
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

