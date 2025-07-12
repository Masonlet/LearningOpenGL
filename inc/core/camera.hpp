#pragma once

#include <GLFW/glfw3.h>

#include "math/constants.hpp"
#include "math/vec4.hpp"
#include "math/mat4.hpp"

#include "core/inputManager.hpp"

class Camera {
public:
	Camera();

	Vec3 GetRight() const;
	Mat4 LookAt() const;

	Mat4 Perspective(const float aspect) const;

	inline Vec3 Pos() const { return pos; }
  inline float Yaw() const { return yaw; }
  inline float Pitch() const { return pitch; }
	inline float MoveSpeed() const { return moveSpeed; }
  inline float MouseSpeed() const { return mouseSpeed; }
  inline float LastX() const { return lastX; }
  inline float LastY() const { return lastY; }
  inline float Paused() const { return paused; }

  inline void MoveForward(const float deltaTime) { pos += front * (deltaTime * moveSpeed); }
  inline void MoveBackward(const float deltaTime) { pos -= front * (deltaTime * moveSpeed); }
  inline void MoveLeft(const float deltaTime) { pos -= GetRight() * (deltaTime * moveSpeed); }
  inline void MoveRight(const float deltaTime) { pos += GetRight() * (deltaTime * moveSpeed); }
  inline void MoveUp(const float deltaTime) { pos += up * (deltaTime * moveSpeed); }
  inline void MoveDown(const float deltaTime) { pos -= up * (deltaTime * moveSpeed); }

  inline void SetYaw(const float yawIn) { yaw = yawIn; }
  inline void SetPitch(const float pitchIn) { pitch = pitchIn; }
  inline void SetX(const float x) { lastX = x; }
  inline void SetY(const float y) { lastY = y; }

  void ProcessInputs(InputManager* input, const float deltaTime);
  void print() const;

private:
	float moveSpeed, mouseSpeed;
	Vec3 pos, front, up;
	float yaw, pitch;
	float lastX, lastY;
	bool paused;

	void ProcessKeyboard(InputManager* input, const float deltaTime);
	void ProcessMouse(InputManager* input);
};
