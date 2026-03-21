#include "vector.h"
#include <math.h>

extern vec3_t addVec3(vec3_t u, vec3_t v);
extern vec3_t scaleVec3(float r, vec3_t u);

extern vec2_t addVec2(vec2_t u, vec2_t v);
extern vec2_t scaleVec2(float r, vec2_t u);

vec2_t rotateVec2(vec2_t v, float theta) {
	return (vec2_t) {
		.x = v.x * cos(theta) - v.y * sin(theta),
		.y = v.x * sin (theta) + v.y * cos(theta)
	};
}

vec3_t rotateVec3(vec3_t v, float theta, char axis) {
	switch (axis) {
		case 'x':
			return (vec3_t) {
				.x = v.x,
				.y = v.y * cos(theta) - v.z * sin(theta),
				.z = v.y * sin(theta) + v.z * cos(theta)
			};
			break;
		case 'y':
			return (vec3_t) {
				.x = v.z * sin(theta) + v.x * cos(theta),
				.y = v.y,
				.z = v.z * cos(theta) - v.x * sin(theta)
			};
			break;
		case 'z':
			return (vec3_t) {
				.x = v.x * cos(theta) - v.y * sin(theta),
				.y = v.x * sin (theta) + v.y * cos(theta),
				.z = v.z
			};
			break;
		default:
			return v;
			break;
	}
}
