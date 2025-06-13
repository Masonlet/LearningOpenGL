#include "mat4.hpp"
#include "constants.hpp"
#include <cmath>
#include <cstdio>

Mat4 Mat4::identity() {
	Mat4 result{};

	result.data[0] = 1.0f;
	result.data[5] = 1.0f;
	result.data[10] = 1.0f;
	result.data[15] = 1.0f;

	return result;
}

Mat4 Mat4::modelMatrix(const Transform& t) {
  return Mat4::translation(t.position) *
  Mat4::rotateX(t.rotation.x) *
  Mat4::rotateY(t.rotation.y) *
  Mat4::rotateZ(t.rotation.z) * 
  Mat4::scale(t.scale);
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

	result.data[5] = c;
	result.data[6] = s;
	result.data[9] = -s;
	result.data[10] = c;
	return result;
}
Mat4 Mat4::rotateY(const float angle) {
	Mat4 result = Mat4::identity();
	const float rad = radians(angle);
	float c = cosf(rad);
	float s = sinf(rad);

	result.data[0] = c;
	result.data[2] = s;
	result.data[8] = -s;
	result.data[10] = c;
	return result;
}
Mat4 Mat4::rotateZ(const float angle) {
	Mat4 result = Mat4::identity();
	const float rad = radians(angle);
  float c = cosf(rad);
  float s = sinf(rad);

  result.data[0] = c;
  result.data[1] = s;
  result.data[4] = -s;
  result.data[5] = c;
  return result;
}
Mat4 Mat4::operator*(const Mat4& b) const {
  Mat4 result{};

  // Column 0
  result.data[0] = data[0] * b.data[0] + data[4] * b.data[1] + data[8]  * b.data[2] + data[12] * b.data[3];
  result.data[1] = data[1] * b.data[0] + data[5] * b.data[1] + data[9]  * b.data[2] + data[13] * b.data[3];
  result.data[2] = data[2] * b.data[0] + data[6] * b.data[1] + data[10] * b.data[2] + data[14] * b.data[3];
  result.data[3] = data[3] * b.data[0] + data[7] * b.data[1] + data[11] * b.data[2] + data[15] * b.data[3];

  // Column 1
  result.data[4] = data[0] * b.data[4] + data[4] * b.data[5] + data[8]  * b.data[6] + data[12] * b.data[7];
  result.data[5] = data[1] * b.data[4] + data[5] * b.data[5] + data[9]  * b.data[6] + data[13] * b.data[7];
  result.data[6] = data[2] * b.data[4] + data[6] * b.data[5] + data[10] * b.data[6] + data[14] * b.data[7];
  result.data[7] = data[3] * b.data[4] + data[7] * b.data[5] + data[11] * b.data[6] + data[15] * b.data[7];

  //// Column 2
  result.data[8]  = data[0] * b.data[8]  + data[4] * b.data[9]  + data[8]  * b.data[10] + data[12] * b.data[11];
  result.data[9]  = data[1] * b.data[8]  + data[5] * b.data[9]  + data[9]  * b.data[10] + data[13] * b.data[11];
  result.data[10] = data[2] * b.data[8]  + data[6] * b.data[9]  + data[10] * b.data[10] + data[14] * b.data[11];
  result.data[11] = data[3] * b.data[8]  + data[7] * b.data[9]  + data[11] * b.data[10] + data[15] * b.data[11];

  //// Column 3
  result.data[12] = data[0] * b.data[12] + data[4] * b.data[13] + data[8]  * b.data[14] + data[12] * b.data[15];
  result.data[13] = data[1] * b.data[12] + data[5] * b.data[13] + data[9]  * b.data[14] + data[13] * b.data[15];
  result.data[14] = data[2] * b.data[12] + data[6] * b.data[13] + data[10] * b.data[14] + data[14] * b.data[15];
  result.data[15] = data[3] * b.data[12] + data[7] * b.data[13] + data[11] * b.data[14] + data[15] * b.data[15];

  return result;
}
Mat4& Mat4::operator*=(const Mat4& b) {
  *this = (*this) * b;
  return *this;
}
bool Mat4::operator==(const Mat4& b) const {
  for (int i = 0; i < 16; ++i)
    if (data[i] != b.data[i])
      return false;
  return true;
}

Transform Mat4::decompose() const {
  Transform t;

  // Position
  t.position.x = data[12];
  t.position.y = data[13];
  t.position.z = data[14];

  // Columns
  Vec3 col0 = {data[0], data[1], data[2] };
  Vec3 col1 = {data[4], data[5], data[6] };
  Vec3 col2 = {data[8], data[9], data[10] };

  // Scale
  t.scale.x = col0.length();
  t.scale.y = col1.length();
  t.scale.z = col2.length();

  // Normalized
  if (t.scale.x != 0) col0 = col0 / t.scale.x;
  if (t.scale.y != 0) col1 = col1 / t.scale.y;
  if (t.scale.z != 0) col2 = col2 / t.scale.z;

  t.rotation.y = asinf(-col0.z);  // Y-axis
  if (cosf(t.rotation.y) != 0.0f) {
    t.rotation.x = atan2f(col1.z, col2.z); // X-axis
    t.rotation.z = atan2f(col0.y, col0.x); // Z-axis
  } else {
    t.rotation.x = atan2f(-col2.x, col1.y); // Gimbal lock case
    t.rotation.z = 0.0f;
  }

  t.rotation.x = degrees(t.rotation.x);
  t.rotation.y = degrees(t.rotation.y);
  t.rotation.z = degrees(t.rotation.z);

  return t;
}


void Mat4::print(const char* name = "Matrix")const{
  printf("--- %s ---\n", name);
  printf("%.4f  %.4f  %.4f  %.4f\n", data[0], data[4], data[8],  data[12]); 
  printf("%.4f  %.4f  %.4f  %.4f\n", data[1], data[5], data[9],  data[13]);
  printf("%.4f  %.4f  %.4f  %.4f\n", data[2], data[6], data[10], data[14]);
  printf("%.4f  %.4f  %.4f  %.4f\n", data[3], data[7], data[11], data[15]);
  printf("-----------\n");
}

