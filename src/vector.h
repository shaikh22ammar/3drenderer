#ifndef VECTOR_H
#define VECTOR_H
#include <math.h>

typedef struct {float x; float y;} vec2_t;
typedef struct {float x; float y; float z;} vec3_t;

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 3-dim vector operations
////////////////////////////////////////////////////////////////////////////////////////////////////

inline vec3_t addVec3(vec3_t u, vec3_t v) {
	return (vec3_t) {
		.x = u.x + v.x,
		.y = u.y + v.y,
		.z = u.z + v.z
	};
}

inline vec3_t scaleVec3(float r, vec3_t u) {
	return (vec3_t) {
		.x = r * u.x,
		.y = r * u.y,
		.z = r * u.z
	};
}

inline vec3_t crossProduct(vec3_t u, vec3_t v) {
	return (vec3_t) {
		.x = u.y * v.z - u.z * v.y,
		.y = u.x * v.z - u.z * v.x,
		.z = u.x * v.y - u.y * v.x
	};
}

inline float dotProductVec3(vec3_t u, vec3_t v) {
	return u.x*v.x + u.y*u.y + u.z*u.z;
}

inline float lenVec3(vec3_t u) {
	return sqrtf(dotProductVec3(u,u));
}

inline vec3_t normalizeVec3(vec3_t u) {
	return scaleVec3(1.0f/sqrtf(u.x*u.x + u.y*u.y + u.z*u.z), u);
}

vec3_t rotateVec3(vec3_t v, float theta, char axis);

/////////////////////////////////////////////////////////////////////////////////////////////////////
// 2-dim vector operations
/////////////////////////////////////////////////////////////////////////////////////////////////////

inline vec2_t addVec2(vec2_t u, vec2_t v) {
	return (vec2_t) {
		.x = u.x + v.x,
		.y = u.y + v.y
	};
}

inline vec2_t scaleVec2(float r, vec2_t u) {
	return (vec2_t) {
		.x = r * u.x,
		.y = r * u.y
	};
}

inline float dotProductVec2(vec2_t u, vec2_t v) {
	return u.x*v.x + u.y*u.y;
}

inline float lenVec2(vec2_t u) {
	return sqrtf(dotProductVec2(u,u));
}

inline vec2_t normalizeVec2(vec2_t u) {
	return scaleVec2(1.0f/sqrtf(u.x*u.x + u.y*u.y), u);
}

vec2_t rotateVec2(vec2_t v, float theta);

#endif
