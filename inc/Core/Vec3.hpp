#pragma once

/*
Vec3
* 3-Dimensional Floating Point Vector
* float x, float y, float z
*/
struct Vec3 {
	float x{0.0f}, y{0.0f}, z{0.0f};

	constexpr Vec3() {};
	constexpr Vec3(const Vec3& b) { x = b.x, y = b.y, z = b.z; };
	constexpr Vec3(float xIn, float yIn, float zIn) { x = xIn, y = yIn, z = zIn; };

	float length() const;
	inline float lengthSquared() const { return x * x + y * y + z * z; }

	Vec3 normalized() const;

	inline Vec3 cross(const Vec3& b) const { return { y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x }; }
	inline float dot(const Vec3& b) const { return x * b.x + y * b.y + z * b.z; }

	inline Vec3 operator-(const Vec3& b) const { return {x - b.x, y - b.y, z - b.z}; }
	inline Vec3 operator*(const Vec3& b) const { return {x * b.x, y * b.y, z * b.z}; }
	inline Vec3& operator+=(const Vec3& b) {
		x += b.x, y += b.y, z += b.z;
		return *this;
	}
	inline Vec3& operator-=(const Vec3& b) {
		x -= b.x, y -= b.y, z -= b.z;
		return *this;
	}

	inline Vec3 operator+(const float b) const { return {x + b, y + b, z + b}; }
	inline Vec3 operator-(const float b) const { return {x - b, y - b, z - b}; }
	inline Vec3 operator*(const float b) const { return {x * b, y * b, z * b}; }
	inline Vec3& operator+=(const float b) {
		x += b, y += b, z += b;
		return *this;
	}
	inline Vec3& operator-=(const float b) {
		x -= b, y -= b, z -= b;
		return *this;
	}

	inline bool operator!=(const Vec3& b) {
		return x != b.x && y != b.y && z != b.z;
	}
};
