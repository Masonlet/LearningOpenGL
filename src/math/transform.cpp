#include "math/transform.hpp"

void Transform::move(const Vec4& position) { pos += position; }
void Transform::rotate(const Vec3& degrees) { rot += degrees; }
void Transform::resize(const Vec3& factor) {
	scale.x *= factor.x;
	scale.y *= factor.y;
	scale.z *= factor.z;
}
