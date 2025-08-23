#include "math/mat4.hpp"
#include "math/constants.hpp"

#include <cmath>
#include <cstdio>

Mat4 Mat4::identity() {
	Mat4 result;
	result.models[0] = 1.0f;
	result.models[5] = 1.0f;
	result.models[10] = 1.0f;
	result.models[15] = 1.0f;
	return result;
}

Mat4 Mat4::modelMatrix(const Transform& t) {
  return Mat4::translation(t.pos) 
         * Mat4::rotateX(t.rot.x)
         * Mat4::rotateY(t.rot.y) 
         * Mat4::rotateZ(t.rot.z) 
         * Mat4::size(t.size);
}

Mat4 Mat4::transpose() const {
	Mat4 result;
	for (int row = 0; row < 4; ++row) 
		for (int col = 0; col < 4; ++col) 
			result.models[col * 4 + row] = models[row * 4 + col];
		
	return result;
}
Mat4 Mat4::inverse() const {
    const float* m = models;

    Mat4 inv; //Im not going to pretend to understand this math, "MESA implementation of the GLU library"
    inv.models[0] = m[5] * m[10] * m[15] -
        m[5] * m[11] * m[14] -
        m[9] * m[6] * m[15] +
        m[9] * m[7] * m[14] +
        m[13] * m[6] * m[11] -
        m[13] * m[7] * m[10];

    inv.models[4] = -m[4] * m[10] * m[15] +
        m[4] * m[11] * m[14] +
        m[8] * m[6] * m[15] -
        m[8] * m[7] * m[14] -
        m[12] * m[6] * m[11] +
        m[12] * m[7] * m[10];

    inv.models[8] = m[4] * m[9] * m[15] -
        m[4] * m[11] * m[13] -
        m[8] * m[5] * m[15] +
        m[8] * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];

    inv.models[12] = -m[4] * m[9] * m[14] +
        m[4] * m[10] * m[13] +
        m[8] * m[5] * m[14] -
        m[8] * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];

    inv.models[1] = -m[1] * m[10] * m[15] +
        m[1] * m[11] * m[14] +
        m[9] * m[2] * m[15] -
        m[9] * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];

    inv.models[5] = m[0] * m[10] * m[15] -
        m[0] * m[11] * m[14] -
        m[8] * m[2] * m[15] +
        m[8] * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];

    inv.models[9] = -m[0] * m[9] * m[15] +
        m[0] * m[11] * m[13] +
        m[8] * m[1] * m[15] -
        m[8] * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];

    inv.models[13] = m[0] * m[9] * m[14] -
        m[0] * m[10] * m[13] -
        m[8] * m[1] * m[14] +
        m[8] * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];

    inv.models[2] = m[1] * m[6] * m[15] -
        m[1] * m[7] * m[14] -
        m[5] * m[2] * m[15] +
        m[5] * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];

    inv.models[6] = -m[0] * m[6] * m[15] +
        m[0] * m[7] * m[14] +
        m[4] * m[2] * m[15] -
        m[4] * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];

    inv.models[10] = m[0] * m[5] * m[15] -
        m[0] * m[7] * m[13] -
        m[4] * m[1] * m[15] +
        m[4] * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];

    inv.models[14] = -m[0] * m[5] * m[14] +
        m[0] * m[6] * m[13] +
        m[4] * m[1] * m[14] -
        m[4] * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];

    inv.models[3] = -m[1] * m[6] * m[11] +
        m[1] * m[7] * m[10] +
        m[5] * m[2] * m[11] -
        m[5] * m[3] * m[10] -
        m[9] * m[2] * m[7] +
        m[9] * m[3] * m[6];

    inv.models[7] = m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];

    inv.models[11] = -m[0] * m[5] * m[11] +
        m[0] * m[7] * m[9] +
        m[4] * m[1] * m[11] -
        m[4] * m[3] * m[9] -
        m[8] * m[1] * m[7] +
        m[8] * m[3] * m[5];

    inv.models[15] = m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];

    float det = m[0] * inv.models[0] + m[1] 
              * inv.models[4] + m[2] 
              * inv.models[8] + m[3] 
              * inv.models[12];

    if (det == 0) return Mat4::identity();
    else          det = 1.0 / det;

    for (int i = 0; i < 16; ++i) inv.models[i] *= det;
    return inv;
}

Mat4 Mat4::translation(const Vec4& t) {
	Mat4 result = Mat4::identity();
	result.models[12] = t.x;
	result.models[13] = t.y;
	result.models[14] = t.z;
	return result;
}
Mat4 Mat4::size(const Vec3& t) {
	Mat4 result = Mat4::identity();
	result.models[0]  = t.x;
	result.models[5]  = t.y;
	result.models[10] = t.z;
	return result;
}

Mat4 Mat4::rotateX(const float angle) {
	const float rad = radians(angle);
	float c = cosf(rad);
	float s = sinf(rad);

  Mat4 result = Mat4::identity();
	result.models[5]  =  c;
	result.models[6]  =  s;
	result.models[9]  = -s;
	result.models[10] =  c;
	return result;
}
Mat4 Mat4::rotateY(const float angle) {
	const float rad = radians(angle);
	float c = cosf(rad);
	float s = sinf(rad);

  Mat4 result = Mat4::identity();
	result.models[0]  =  c;
	result.models[2]  =  s;
	result.models[8]  = -s;
	result.models[10] =  c;
	return result;
}
Mat4 Mat4::rotateZ(const float angle) {
	const float rad = radians(angle);
  float c = cosf(rad);
  float s = sinf(rad);

  Mat4 result = Mat4::identity();
  result.models[0] =  c;
  result.models[1] =  s;
  result.models[4] = -s;
  result.models[5] =  c;
  return result;
}

Transform Mat4::decompose() const {
  Transform t;

  t.pos.x = models[12];
  t.pos.y = models[13];
  t.pos.z = models[14];

  Vec3 col0 = {models[0], models[1], models[2] };
  Vec3 col1 = {models[4], models[5], models[6] };
  Vec3 col2 = {models[8], models[9], models[10] };

  t.size.x = col0.length();
  t.size.y = col1.length();
  t.size.z = col2.length();

  // Normalized
  if (t.size.x != 0) col0 = col0 / t.size.x;
  if (t.size.y != 0) col1 = col1 / t.size.y;
  if (t.size.z != 0) col2 = col2 / t.size.z;

  t.rot.y = asinf(-col0.z);  // Y-axis
  if (cosf(t.rot.y) != 0.0f) {
    t.rot.x = atan2f(col1.z, col2.z); // X-axis
    t.rot.z = atan2f(col0.y, col0.x); // Z-axis
  } else {
    t.rot.x = atan2f(-col2.x, col1.y); // Gimbal lock case
    t.rot.z = 0.0f;
  }

  t.rot.x = degrees(t.rot.x);
  t.rot.y = degrees(t.rot.y);
  t.rot.z = degrees(t.rot.z);

  return t;
}

void Mat4::print(const char* name)const{
  printf("--- %s ---\n", name);
  printf("%.4f  %.4f  %.4f  %.4f\n", models[0], models[4], models[8],  models[12]); 
  printf("%.4f  %.4f  %.4f  %.4f\n", models[1], models[5], models[9],  models[13]);
  printf("%.4f  %.4f  %.4f  %.4f\n", models[2], models[6], models[10], models[14]);
  printf("%.4f  %.4f  %.4f  %.4f\n", models[3], models[7], models[11], models[15]);
  printf("-----------\n");
}

Mat4 Mat4::operator*(const Mat4& b) const {
  Mat4 result{};

  // Column 0
  result.models[0] = models[0] * b.models[0] + models[4] * b.models[1] + models[8]  * b.models[2] + models[12] * b.models[3];
  result.models[1] = models[1] * b.models[0] + models[5] * b.models[1] + models[9]  * b.models[2] + models[13] * b.models[3];
  result.models[2] = models[2] * b.models[0] + models[6] * b.models[1] + models[10] * b.models[2] + models[14] * b.models[3];
  result.models[3] = models[3] * b.models[0] + models[7] * b.models[1] + models[11] * b.models[2] + models[15] * b.models[3];

  // Column 1
  result.models[4] = models[0] * b.models[4] + models[4] * b.models[5] + models[8]  * b.models[6] + models[12] * b.models[7];
  result.models[5] = models[1] * b.models[4] + models[5] * b.models[5] + models[9]  * b.models[6] + models[13] * b.models[7];
  result.models[6] = models[2] * b.models[4] + models[6] * b.models[5] + models[10] * b.models[6] + models[14] * b.models[7];
  result.models[7] = models[3] * b.models[4] + models[7] * b.models[5] + models[11] * b.models[6] + models[15] * b.models[7];

  //// Column 2
  result.models[8]  = models[0] * b.models[8] + models[4] * b.models[9] + models[8]  * b.models[10] + models[12] * b.models[11];
  result.models[9]  = models[1] * b.models[8] + models[5] * b.models[9] + models[9]  * b.models[10] + models[13] * b.models[11];
  result.models[10] = models[2] * b.models[8] + models[6] * b.models[9] + models[10] * b.models[10] + models[14] * b.models[11];
  result.models[11] = models[3] * b.models[8] + models[7] * b.models[9] + models[11] * b.models[10] + models[15] * b.models[11];

  //// Column 3
  result.models[12] = models[0] * b.models[12] + models[4] * b.models[13] + models[8]  * b.models[14] + models[12] * b.models[15];
  result.models[13] = models[1] * b.models[12] + models[5] * b.models[13] + models[9]  * b.models[14] + models[13] * b.models[15];
  result.models[14] = models[2] * b.models[12] + models[6] * b.models[13] + models[10] * b.models[14] + models[14] * b.models[15];
  result.models[15] = models[3] * b.models[12] + models[7] * b.models[13] + models[11] * b.models[14] + models[15] * b.models[15];

  return result;
}
Vec4 Mat4::operator*(const Vec4& v) const {
  Vec4 result;
  result.x = models[0] * v.x + models[4] * v.y + models[8] * v.z + models[12] * v.w;
  result.y = models[1] * v.x + models[5] * v.y + models[9] * v.z + models[13] * v.w;
  result.z = models[2] * v.x + models[6] * v.y + models[10] * v.z + models[14] * v.w;
  result.w = models[3] * v.x + models[7] * v.y + models[11] * v.z + models[15] * v.w;
  return result;
}

Mat4& Mat4::operator*=(const Mat4& b) {
  *this = (*this) * b;
  return *this;
}
bool Mat4::operator==(const Mat4& b) const {
  for (int i = 0; i < 16; ++i)
    if (models[i] != b.models[i])
      return false;

  return true;
}


