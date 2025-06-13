#include "vec4.hpp"
#include <cmath>

float Vec4::length() const { 
	return sqrt(x * x + y * y + z * z + w * w); 
}

Vec4 Vec4::normalized() const {
	float len = lengthSquared();

	if (len < 1e-6f) 
		return {0.0f, 0.0f, 0.0f, 0.0f};

	len = sqrt(len);
	return {x / len, y / len, z / len, w / len};
}
