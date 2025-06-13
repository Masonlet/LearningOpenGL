#pragma once

#include "init.hpp"
#include "vec3.hpp"
#include "mat4.hpp"

#include <glad.h> // If "GLFW_INCLUDE_NONE" was not defined this would need to be included before glfw3.h

constexpr float MOVE_SPEED{25.0f};
constexpr float CAMERA_SPEED{0.1f};

constexpr float DEFAULT_FOV{75.0f};
constexpr float NEAR_PLANE{0.1f};
constexpr float FAR_PLANE{10000.0f};

constexpr float DEFAULT_YAW{-90.0f};
constexpr float DEFAULT_PITCH{0.0f};

constexpr Vec3 INITIAL_POS{0.0f, 0.0f, -10.0f};
constexpr Vec3 INITIAL_TARGET{0.0f, 00.0f, -1.0f};
constexpr Vec3 WORLD_UP{0.0f, 1.0f, 0.0f};
	
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

  void ProcessInputs(GLFWwindow* window, Vec3& pos, Vec3& rot, Vec3& scale, const float deltaTime);
  void print();

private:
	float moveSpeed, mouseSpeed;
	Vec3 pos, front, up;
	float yaw, pitch;
	float lastX, lastY;
	bool paused;

	void ProcessKeyboard(GLFWwindow* window, const float deltaTime);
	void ProcessMouse(GLFWwindow* window, const double xpos, const double ypos);
};
