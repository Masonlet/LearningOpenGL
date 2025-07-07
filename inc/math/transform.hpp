#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"

struct Transform {
	Vec4 position{0.0f, 0.0f, 0.0f, 0.0f};
	Vec3 rotation{0.0f, 0.0f, 0.0f};
	Vec3 scale{10.0f, 10.0f, 10.0f};

	inline void move(const Vec4& pos) { position += pos; }

	inline void rotate(const Vec3& degrees) { rotation += degrees; }

	inline void resize(const Vec3& factor) {
		scale.x *= factor.x;
		scale.y *= factor.y;
		scale.z *= factor.z;
	}
};
