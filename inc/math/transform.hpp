#pragma once

#include "math/vec3.hpp"
#include "math/vec4.hpp"

struct Transform {
	Vec4 pos{0.0f};
	Vec3 rot{0.0f};
	Vec3 size{1.0f};
};
