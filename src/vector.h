#ifndef VECTOR_H
#define VECTOR_H

typedef struct {float x; float y;} vec2_t;
typedef struct {float x; float y; float z;} vec3_t;

inline vec3_t addVec3(vec3_t u, vec3_t v) {
	return (vec3_t) {
		.x = u.x + v.y,
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

inline vec2_t addVec2(vec2_t u, vec2_t v) {
	return (vec2_t) {
		.x = u.x + v.y,
		.y = u.y + v.y
	};
}

inline vec2_t scaleVec2(float r, vec2_t u) {
	return (vec2_t) {
		.x = r * u.x,
		.y = r * u.y
	};
}

#endif
