#pragma once

#include "math/constants.hpp"
#include "math/vec3.hpp"
#include "math/mat4.hpp"

#include "core/inputManager.hpp"
#include "core/modelInstance.hpp"

#include <map>
#include <string>

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
	inline unsigned int Type() const { return type; }
  inline float MoveDistance() const { return moveDistance; }

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
  inline void SetFov(const float fovIn) { fov = fovIn; }
  inline void SetNear(const float nearIn) { nearPlane = nearIn; }
  inline void SetFar(const float farIn) { farPlane = farIn; }
  inline void SetMoveSpeed(const float speedIn) { moveSpeed = speedIn; }
	inline void SetType(const unsigned int typeIn) { type = typeIn; }
  inline void SetMoveDistance(const float moveDistanceIn) { moveDistance = moveDistanceIn; }

  void ProcessInputs(InputManager* input, const float deltaTime);
  void Print() const;

private:
  unsigned int type;
	float moveSpeed, mouseSpeed, moveDistance;
	Vec3 pos, front, up;
	float yaw, pitch;
	float lastX, lastY;
  float fov;
  float nearPlane, farPlane;
  bool paused;

	void UpdateFreeCam(InputManager* input, const float deltaTime);
  void UpdateDungeonCam(InputManager* input, const float deltaTime);
  void UpdateModernCam(InputManager* input, const float deltaTime);

  void ProcessKeyboard(InputManager* input, const float deltaTime);
  void ProcessMouse(InputManager* input);
};
