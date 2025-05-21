#pragma once

#include "Vec3.hpp"

struct Transform {
	Vec3 position{0.0f, 0.0f, 0.0f};
	Vec3 rotation{0.0f, 0.0f, 0.0f};
	Vec3 size{10.0f, 10.0f, 10.0f};

	inline void move(const Vec3& pos) { position += pos; }

	inline void rotate(const Vec3& degrees) { rotation += degrees; }

	inline void scale(const Vec3& factor) {
		size.x *= factor.x;
		size.y *= factor.y;
		size.z *= factor.z;
	}
};
