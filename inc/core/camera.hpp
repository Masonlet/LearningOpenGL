#pragma once

#include "math/vec3.hpp"
#include "math/mat4.hpp"
#include "core/inputManager.hpp"
#include <string>

class Camera {
public:
	Camera();

	Vec3 GetRight() const;
	Mat4 LookAt() const;
	Mat4 Perspective(const float aspect) const;

  inline std::string getName() const { return name; }
	inline Vec3 getPos() const { return pos; }
  inline float getYaw() const { return yaw; }
  inline float getPitch() const { return pitch; }
	inline float getMoveSpeed() const { return moveSpeed; }
  inline float getMouseSpeed() const { return mouseSpeed; }
  inline float getLastX() const { return lastX; }
  inline float getLastY() const { return lastY; }
  inline float getFov() const { return fov; }
  inline float getNearPlane() const { return nearPlane; }
  inline float getFarPlane() const { return farPlane; }
  inline float getPaused() const { return paused; }
  inline float getMoveDistance() const { return moveDistance; }
  inline unsigned int getType() const { return type; }

	inline void setName(const std::string& nameIn) { name = nameIn; }
  inline void setPos(const Vec3& posIn) { pos = posIn; }
  inline void setYaw(const float yawIn) { yaw = yawIn; }
  inline void setPitch(const float pitchIn) { pitch = pitchIn; }
  inline void setX(const float x) { lastX = x; }
  inline void setY(const float y) { lastY = y; }
  inline void setNear(const float nearIn) { nearPlane = nearIn; }
  inline void setFar(const float farIn) { farPlane = farIn; }
  inline void setMoveSpeed(const float speedIn) { moveSpeed = speedIn; }
	inline void setType(const unsigned int typeIn) { type = typeIn; }
  inline void setMoveDistance(const float moveDistanceIn) { moveDistance = moveDistanceIn; }
  void setFov(const float fovIn);

  inline void moveForward(const float deltaTime) { pos += front * (deltaTime * moveSpeed); }
  inline void moveBackward(const float deltaTime) { pos -= front * (deltaTime * moveSpeed); }
  inline void moveLeft(const float deltaTime) { pos -= GetRight() * (deltaTime * moveSpeed); }
  inline void moveRight(const float deltaTime) { pos += GetRight() * (deltaTime * moveSpeed); }
  inline void moveUp(const float deltaTime) { pos += up * (deltaTime * moveSpeed); }
  inline void moveDown(const float deltaTime) { pos -= up * (deltaTime * moveSpeed); }

  void processInputs(InputManager* input, const float deltaTime);
  void print() const;

private:
  std::string name;
  unsigned int type;
	float moveSpeed, mouseSpeed, moveDistance;
	Vec3 pos, front, up;
	float yaw, pitch;
	float lastX, lastY;
  float fov;
  float nearPlane, farPlane;
  bool paused;

	void updateFreeCam(InputManager* input, const float deltaTime);
  void updateDungeonCam(InputManager* input, const float deltaTime);
  void updateModernCam(InputManager* input, const float deltaTime);

  void processKeyboard(InputManager* input, const float deltaTime);
  void processMouse(InputManager* input);
};
