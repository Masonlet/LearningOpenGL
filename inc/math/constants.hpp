#pragma once

#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"

constexpr unsigned int NUMBEROFLIGHTS{ 50 };

constexpr Vec2 DEFAULT_SIZE_2D{ 1.0f, 1.0f };
constexpr Vec3 DEFAULT_SIZE_3D{ 1.0f, 1.0f, 1.0f };
constexpr Vec3 DEFAULT_ROTATION{ 0.0f, 0.0f, 0.0f };
constexpr Vec3 DEFAULT_SCALE{ 1.0f, 1.0f, 1.0f };

constexpr float MOVE_SPEED{ 25.0f };
constexpr float CAMERA_SPEED{ 0.1f };

constexpr float DEFAULT_FOV{ 60.0f };
constexpr float NEAR_PLANE{ 0.1f };
constexpr float FAR_PLANE{ 10000.0f };

constexpr float DEFAULT_YAW{ -90.0f };
constexpr float DEFAULT_PITCH{ 0.0f };
constexpr Vec3 INITIAL_POS{ 0.0f, 0.0f, -10.0f };
constexpr Vec3 INITIAL_TARGET{ 0.0f, 0.0f, -1.0f };
constexpr Vec3 WORLD_UP{ 0.0f, 1.0f, 0.0f };

constexpr Vec3 DEFAULT_NORMAL{ 0.0f, 0.0f, 0.0f };
constexpr Vec4 DEFAULT_COLOUR{ 0.0f, 1.0f, 0.0f, 1.0f };

constexpr float DEG_TO_RAD{0.01745329251994329576923690768489f};
inline float radians(float degrees) { return degrees * DEG_TO_RAD; }

constexpr float RAD_TO_DEG{57.295779513082320876798154814105f};
inline float degrees(float radians) { return radians * RAD_TO_DEG; }
