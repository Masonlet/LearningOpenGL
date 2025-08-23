#pragma once

#include "math/vec3.hpp"
#include "math/mat4.hpp"
#include "core/inputManager.hpp"
#include <string>

struct Camera {
  std::string name;
  unsigned int type{ 0 };
  float moveSpeed{ 25.0f }, mouseSpeed{ 0.1f }, moveDistance{ 0 };
  Vec3 pos{ 0.0f, 0.0f, 0.0f }, front{ 0.0f, 0.0f, -1.0f }, up{ 0.0f, 1.0f, 0.0f };
  float fov{ 60.0f };
  float yaw{ 0.0f }, pitch{ 0.0f };
  float lastX{ 0.0f }, lastY{ 0.0f };
  float nearPlane{ 0.1f }, farPlane{ 10000.0f };
  bool enabled{ true }, paused{ false };
  
  void processInputs(InputManager* input, const float deltaTime);
  void setFov(const float fovIn);

	Vec3 GetRight() const;
	Mat4 LookAt() const;
	Mat4 Perspective(const float aspect) const;
  void print() const;

private:
  inline void moveForward(const float deltaTime) { pos += front * (deltaTime * moveSpeed); }
  inline void moveBackward(const float deltaTime) { pos -= front * (deltaTime * moveSpeed); }
  inline void moveLeft(const float deltaTime) { pos -= GetRight() * (deltaTime * moveSpeed); }
  inline void moveRight(const float deltaTime) { pos += GetRight() * (deltaTime * moveSpeed); }
  inline void moveUp(const float deltaTime) { pos += up * (deltaTime * moveSpeed); }
  inline void moveDown(const float deltaTime) { pos -= up * (deltaTime * moveSpeed); }

  void updateFreeCam(InputManager* input, const float deltaTime);
  void updateDungeonCam(InputManager* input, const float deltaTime);
  void updateModernCam(InputManager* input, const float deltaTime);

  void processKeyboard(InputManager* input, const float deltaTime);
  void processMouse(InputManager* input);
};
