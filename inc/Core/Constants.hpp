#pragma once

constexpr float DEG_TO_RAD{0.01745329251994329576923690768489f};
inline float radians(float degrees) { return degrees * DEG_TO_RAD; }
