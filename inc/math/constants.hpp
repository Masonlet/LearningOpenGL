#pragma once

constexpr float DEG_TO_RAD{0.01745329251994329576923690768489f};
inline float radians(float degrees) { return degrees * DEG_TO_RAD; }

constexpr float RAD_TO_DEG{57.295779513082320876798154814105f};
inline float degrees(float radians) { return radians * RAD_TO_DEG; }
