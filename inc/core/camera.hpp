#pragma once

#include "math/constants.hpp"
#include "math/vec3.hpp"
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
  inline float Fov() const { return fov; }
  inline float Near() const { return nearPlane; }
  inline float Far() const { return farPlane; }
  inline float Paused() const { return paused; }

  inline void MoveForward(const float deltaTime) { pos += front * (deltaTime * moveSpeed); }
  inline void MoveBackward(const float deltaTime) { pos -= front * (deltaTime * moveSpeed); }
  inline void MoveLeft(const float deltaTime) { pos -= GetRight() * (deltaTime * moveSpeed); }
  inline void MoveRight(const float deltaTime) { pos += GetRight() * (deltaTime * moveSpeed); }
  inline void MoveUp(const float deltaTime) { pos += up * (deltaTime * moveSpeed); }
  inline void MoveDown(const float deltaTime) { pos -= up * (deltaTime * moveSpeed); }

  inline void SetPos(const Vec3 posIn) { pos = posIn; }
  inline void SetYaw(const float yawIn) { yaw = yawIn; }
  inline void SetPitch(const float pitchIn) { pitch = pitchIn; }
  inline void SetX(const float x) { lastX = x; }
  inline void SetY(const float y) { lastY = y; }
  inline void SetFov(float fovIn) { fov = fovIn; }
  inline void SetNear(float nearIn) { nearPlane = nearIn; }
  inline void SetFar(float farIn) { farPlane = farIn; }
  inline void SetMoveSpeed(float speedIn) { moveSpeed = speedIn; }

  void ProcessInputs(InputManager* input, const float deltaTime);
  void print() const;

private:
	float moveSpeed, mouseSpeed;
	Vec3 pos, front, up;
	float yaw, pitch;
	float lastX, lastY;
  float fov;
  float nearPlane, farPlane;
  bool paused;

	void ProcessKeyboard(InputManager* input, const float deltaTime);
	void ProcessMouse(InputManager* input);
};
