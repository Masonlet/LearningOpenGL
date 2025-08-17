#include "math/mat4.hpp"
#include "math/constants.hpp"

#include <cmath>
#include <cstdio>

Mat4 Mat4::identity() {
	Mat4 result;
	result.data[0] = 1.0f;
	result.data[5] = 1.0f;
	result.data[10] = 1.0f;
	result.data[15] = 1.0f;
	return result;
}

Mat4 Mat4::modelMatrix(const Transform& t) {
  return Mat4::translation(t.position) 
         * Mat4::rotateX(t.rotation.x)
         * Mat4::rotateY(t.rotation.y) 
         * Mat4::rotateZ(t.rotation.z) 
         * Mat4::scale(t.scale);
}

Mat4 Mat4::transpose() const {
	Mat4 result;
	for (int row = 0; row < 4; ++row) 
		for (int col = 0; col < 4; ++col) 
			result.data[col * 4 + row] = data[row * 4 + col];
		
	return result;
}
Mat4 Mat4::inverse() const {
    const float* m = data;

    Mat4 inv; //Im not going to pretend to understand this math, "MESA implementation of the GLU library"
    inv.data[0] = m[5] * m[10] * m[15] -
        m[5] * m[11] * m[14] -
        m[9] * m[6] * m[15] +
        m[9] * m[7] * m[14] +
        m[13] * m[6] * m[11] -
        m[13] * m[7] * m[10];

    inv.data[4] = -m[4] * m[10] * m[15] +
        m[4] * m[11] * m[14] +
        m[8] * m[6] * m[15] -
        m[8] * m[7] * m[14] -
        m[12] * m[6] * m[11] +
        m[12] * m[7] * m[10];

    inv.data[8] = m[4] * m[9] * m[15] -
        m[4] * m[11] * m[13] -
        m[8] * m[5] * m[15] +
        m[8] * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];

    inv.data[12] = -m[4] * m[9] * m[14] +
        m[4] * m[10] * m[13] +
        m[8] * m[5] * m[14] -
        m[8] * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];

    inv.data[1] = -m[1] * m[10] * m[15] +
        m[1] * m[11] * m[14] +
        m[9] * m[2] * m[15] -
        m[9] * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];

    inv.data[5] = m[0] * m[10] * m[15] -
        m[0] * m[11] * m[14] -
        m[8] * m[2] * m[15] +
        m[8] * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];

    inv.data[9] = -m[0] * m[9] * m[15] +
        m[0] * m[11] * m[13] +
        m[8] * m[1] * m[15] -
        m[8] * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];

    inv.data[13] = m[0] * m[9] * m[14] -
        m[0] * m[10] * m[13] -
        m[8] * m[1] * m[14] +
        m[8] * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];

    inv.data[2] = m[1] * m[6] * m[15] -
        m[1] * m[7] * m[14] -
        m[5] * m[2] * m[15] +
        m[5] * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];

    inv.data[6] = -m[0] * m[6] * m[15] +
        m[0] * m[7] * m[14] +
        m[4] * m[2] * m[15] -
        m[4] * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];

    inv.data[10] = m[0] * m[5] * m[15] -
        m[0] * m[7] * m[13] -
        m[4] * m[1] * m[15] +
        m[4] * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];

    inv.data[14] = -m[0] * m[5] * m[14] +
        m[0] * m[6] * m[13] +
        m[4] * m[1] * m[14] -
        m[4] * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];

    inv.data[3] = -m[1] * m[6] * m[11] +
        m[1] * m[7] * m[10] +
        m[5] * m[2] * m[11] -
        m[5] * m[3] * m[10] -
        m[9] * m[2] * m[7] +
        m[9] * m[3] * m[6];

    inv.data[7] = m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];

    inv.data[11] = -m[0] * m[5] * m[11] +
        m[0] * m[7] * m[9] +
        m[4] * m[1] * m[11] -
        m[4] * m[3] * m[9] -
        m[8] * m[1] * m[7] +
        m[8] * m[3] * m[5];

    inv.data[15] = m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];

    float det = m[0] * inv.data[0] + m[1] 
              * inv.data[4] + m[2] 
              * inv.data[8] + m[3] 
              * inv.data[12];

    if (det == 0) return Mat4::identity();
    else          det = 1.0 / det;

    for (int i = 0; i < 16; ++i) inv.data[i] *= det;
    return inv;
}

Mat4 Mat4::translation(const Vec4& t) {
	Mat4 result = Mat4::identity();
	result.data[12] = t.x;
	result.data[13] = t.y;
	result.data[14] = t.z;
	return result;
}
Mat4 Mat4::scale(const Vec3& t) {
	Mat4 result = Mat4::identity();
	result.data[0]  = t.x;
	result.data[5]  = t.y;
	result.data[10] = t.z;
	return result;
}

Mat4 Mat4::rotateX(const float angle) {
	const float rad = radians(angle);
	float c = cosf(rad);
	float s = sinf(rad);

  Mat4 result = Mat4::identity();
	result.data[5]  =  c;
	result.data[6]  =  s;
	result.data[9]  = -s;
	result.data[10] =  c;
	return result;
}
Mat4 Mat4::rotateY(const float angle) {
	const float rad = radians(angle);
	float c = cosf(rad);
	float s = sinf(rad);

  Mat4 result = Mat4::identity();
	result.data[0]  =  c;
	result.data[2]  =  s;
	result.data[8]  = -s;
	result.data[10] =  c;
	return result;
}
Mat4 Mat4::rotateZ(const float angle) {
	const float rad = radians(angle);
  float c = cosf(rad);
  float s = sinf(rad);

  Mat4 result = Mat4::identity();
  result.data[0] =  c;
  result.data[1] =  s;
  result.data[4] = -s;
  result.data[5] =  c;
  return result;
}

Transform Mat4::decompose() const {
  Transform t;

  t.position.x = data[12];
  t.position.y = data[13];
  t.position.z = data[14];

  Vec3 col0 = {data[0], data[1], data[2] };
  Vec3 col1 = {data[4], data[5], data[6] };
  Vec3 col2 = {data[8], data[9], data[10] };

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

void Mat4::print(const char* name)const{
  printf("--- %s ---\n", name);
  printf("%.4f  %.4f  %.4f  %.4f\n", data[0], data[4], data[8],  data[12]); 
  printf("%.4f  %.4f  %.4f  %.4f\n", data[1], data[5], data[9],  data[13]);
  printf("%.4f  %.4f  %.4f  %.4f\n", data[2], data[6], data[10], data[14]);
  printf("%.4f  %.4f  %.4f  %.4f\n", data[3], data[7], data[11], data[15]);
  printf("-----------\n");
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
  result.data[8]  = data[0] * b.data[8] + data[4] * b.data[9] + data[8]  * b.data[10] + data[12] * b.data[11];
  result.data[9]  = data[1] * b.data[8] + data[5] * b.data[9] + data[9]  * b.data[10] + data[13] * b.data[11];
  result.data[10] = data[2] * b.data[8] + data[6] * b.data[9] + data[10] * b.data[10] + data[14] * b.data[11];
  result.data[11] = data[3] * b.data[8] + data[7] * b.data[9] + data[11] * b.data[10] + data[15] * b.data[11];

  //// Column 3
  result.data[12] = data[0] * b.data[12] + data[4] * b.data[13] + data[8]  * b.data[14] + data[12] * b.data[15];
  result.data[13] = data[1] * b.data[12] + data[5] * b.data[13] + data[9]  * b.data[14] + data[13] * b.data[15];
  result.data[14] = data[2] * b.data[12] + data[6] * b.data[13] + data[10] * b.data[14] + data[14] * b.data[15];
  result.data[15] = data[3] * b.data[12] + data[7] * b.data[13] + data[11] * b.data[14] + data[15] * b.data[15];

  return result;
}
Vec4 Mat4::operator*(const Vec4& v) const {
  Vec4 result;
  result.x = data[0] * v.x + data[4] * v.y + data[8] * v.z + data[12] * v.w;
  result.y = data[1] * v.x + data[5] * v.y + data[9] * v.z + data[13] * v.w;
  result.z = data[2] * v.x + data[6] * v.y + data[10] * v.z + data[14] * v.w;
  result.w = data[3] * v.x + data[7] * v.y + data[11] * v.z + data[15] * v.w;
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


