#pragma once

#include "starletmath/vec3.hpp"
#include "starletmath/mat4.hpp"
#include "core/inputManager.hpp"
#include <string>

struct Camera {
  std::string name;
  float moveSpeed{ 25.0f }, mouseSpeed{ 0.1f };
  Vec3 pos{ 0.0f, 0.0f, 0.0f }, front{ 0.0f, 0.0f, -1.0f }, up{ 0.0f, 1.0f, 0.0f };
  float fov{ 60.0f };
  float yaw{ 0.0f }, pitch{ 0.0f };
  float nearPlane{ 0.1f }, farPlane{ 10000.0f };
  bool enabled{ true }, paused{ false };
  float lastX{ 0.0f }, lastY{ 0.0f };
  
  Vec3 GetRight() const { return front.cross(WORLD_UP).normalized(); }
};
