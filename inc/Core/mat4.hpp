#pragma once

#include "transform.hpp"

struct Mat4 {
	float data[16]{0.0f};

	inline const float* ptr() const { return data; }
	inline float* ptr() { return data; }

	static Mat4 identity();
	static Mat4 modelMatrix(const Transform& t);

  static Mat4 translation(const Vec3& t);
  static Mat4 scale(const Vec3& t);

  static Mat4 rotateX(const float angle);
  static Mat4 rotateY(const float angle);
  static Mat4 rotateZ(const float angle);

  Mat4 operator*(const Mat4& b) const;
  Mat4& operator*=(const Mat4& b);
  bool operator==(const Mat4& b) const;

  Transform decompose() const;
  void print(const char* name)const;
};
