#include "math/vec4.hpp"
#include "math/vec3.hpp"

#include <cmath>

Vec4::Vec4(const Vec3& v, float wIn) : x(v.x), y(v.y), z(v.z), w(wIn) {}
Vec4::Vec4(unsigned int x, const Vec3& v) : x(x), y(v.x), z(v.y), w(v.z) {}

float Vec4::length() const { 
	return sqrt(x * x + y * y + z * z + w * w); 
}

Vec4 Vec4::normalized() const {
	float len = lengthSquared();
	if (len < 1e-6f) return {0.0f, 0.0f, 0.0f, 0.0f};

	len = sqrt(len);
	return {x / len, y / len, z / len, w / len};
}
