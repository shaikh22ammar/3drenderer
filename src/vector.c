#include "vector.h"
#include <math.h>

extern vec3_t addVec3(vec3_t u, vec3_t v);
extern vec3_t scaleVec3(float r, vec3_t u);
extern vec2_t addVec2(vec2_t u, vec2_t v);
extern vec3_t crossProduct(vec3_t u, vec3_t v);
extern float dotProductVec3(vec3_t u, vec3_t v);
extern float lenVec3(vec3_t u);
extern vec3_t normalizeVec3(vec3_t u);

vec3_t rotateVec3(vec3_t v, float theta, char axis) {
	switch (axis) {
		case 'x':
			return (vec3_t) {
				.x = v.x,
				.y = v.y * cosf(theta) - v.z * sinf(theta),
				.z = v.y * sinf(theta) + v.z * cosf(theta)
			};
			break;
		case 'y':
			return (vec3_t) {
				.x = v.z * sinf(theta) + v.x * cosf(theta),
				.y = v.y,
				.z = v.z * cosf(theta) - v.x * sinf(theta)
			};
			break;
		case 'z':
			return (vec3_t) {
				.x = v.x * cosf(theta) - v.y * sinf(theta),
				.y = v.x * sinf (theta) + v.y * cosf(theta),
				.z = v.z
			};
			break;
		default:
			return v;
			break;
	}
}

extern vec2_t addVec2(vec2_t u, vec2_t v);
extern vec2_t scaleVec2(float r, vec2_t u);
extern float dotProductVec2(vec2_t u, vec2_t v);
extern float lenVec2(vec2_t u);
extern vec2_t normalizeVec2(vec2_t u);

vec2_t rotateVec2(vec2_t v, float theta) {
	return (vec2_t) {
		.x = v.x * cosf(theta) - v.y * sinf(theta),
		.y = v.x * sinf (theta) + v.y * cosf(theta)
	};
}
