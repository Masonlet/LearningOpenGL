#include "Vec3.hpp"
#include <cmath>

float Vec3::length() const { 
	return sqrt(x * x + y * y + z * z); 
}
Vec3 Vec3::normalized() const {
	float len = lengthSquared();

	if (len < 1e-6f) 
		return {0.0f, 0.0f, 0.0f};

	len = sqrt(len);
	return {x / len, y / len, z / len};
}