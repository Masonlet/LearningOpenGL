#include "Mat4.hpp"
#include "Constants.hpp"
#include <cmath>

Mat4 Mat4::identity() {
	Mat4 result{};

	result.data[0] = 1.0f; 
	result.data[5] = 1.0f; 
	result.data[10] = 1.0f;
	result.data[15] = 1.0f;

	return result;
}

Mat4 Mat4::modelMatrix(const Transform& t) {
	return Mat4::scale(t.size) *
	       Mat4::rotateX(t.rotation.x) *
	       Mat4::rotateY(t.rotation.y) *
	       Mat4::rotateZ(t.rotation.z) *
	       Mat4::translation(t.position);
}

Mat4 Mat4::translation(const Vec3& t) {
	Mat4 result = Mat4::identity();

	result.data[12] = t.x;
	result.data[13] = t.y;
	result.data[14] = t.z;

	return result;
}
Mat4 Mat4::scale(const Vec3& t) {
	Mat4 result = Mat4::identity();

	result.data[0] = t.x;
	result.data[5] = t.y;
	result.data[10] = t.z;

	return result;
}
Mat4 Mat4::rotateX(const float angle) {
	Mat4 result = Mat4::identity();
	const float rad = radians(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	result.data[5] =  c;
	result.data[6] =  s;
	result.data[9] = -s;
	result.data[10] = c;
	return result;
}
Mat4 Mat4::rotateY(const float angle) {
	Mat4 result = Mat4::identity();
	const float rad = radians(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	result.data[0] =  c;
	result.data[2] =  s;
	result.data[8] = -s;
	result.data[10] = c;
	return result;
}
Mat4 Mat4::rotateZ(const float angle) {
	Mat4 result = Mat4::identity();
	const float rad = radians(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	result.data[0] =  c;
	result.data[1] =  s;
	result.data[4] = -s;
	result.data[5] =  c;
	return result;
}