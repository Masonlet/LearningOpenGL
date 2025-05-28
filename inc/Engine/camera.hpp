#pragma once

#include "init.hpp"
#include "Vec3.hpp"
#include "mat4.hpp"

#include <glad/glad.h> // If "GLFW_INCLUDE_NONE" was not defined this would need to be included before glfw3.h

constexpr float MOVE_SPEED{25.0f};
constexpr float CAMERA_SPEED{0.1f};

constexpr float DEFAULT_FOV{75.0f};
constexpr float NEAR_PLANE{0.1f};
constexpr float FAR_PLANE{10000.0f};

constexpr float DEFAULT_YAW{0.0f};
constexpr float DEFAULT_PITCH{0.0f};

constexpr Vec3 INITIAL_POS{0.0f, 0.0f, -10.0f};
constexpr Vec3 INITIAL_TARGET{0.0f, 00.0f, -1.0f};
constexpr Vec3 WORLD_UP{0.0f, 1.0f, 0.0f};
	
struct Camera {
	float moveSpeed, mouseSpeed;
	Vec3 pos, front, up;
	float yaw, pitch;
	float lastX, lastY;
	bool paused;

	void ProcessMouse(GLFWwindow* window, const double xpos, const double ypos);
	void ProcessKeyboard(GLFWwindow* window, const float deltaTime);

public:
	Camera();

	Vec3 GetRight() const;
	Mat4 LookAt() const;

	Mat4 Perspective(const float aspect) const;

	inline Vec3 Pos() const { return pos; }
	inline float MoveSpeed() const { return moveSpeed; }
	inline float MouseSpeed() const { return mouseSpeed; }
	inline float Yaw() const { return yaw; }
	inline float Pitch() const { return pitch; }
	inline float LastX() const { return lastX; }
	inline float LastY() const { return lastY; }
	inline float Paused() const { return paused; }

	void MoveForward(const float amount);
	void MoveBackward(const float amount);
	void MoveLeft(const float amount);
	void MoveRight(const float amount);

	void MoveUp(const float amount);
	void MoveDown(const float amount);

	inline void SetYaw(const float yawIn) { yaw = yawIn; }
	inline void SetPitch(const float pitchIn) { pitch = pitchIn; }
	inline void SetX(const float x) { lastX = x; }
	inline void SetY(const float y) { lastY = y; }

	void ProcessInputs(GLFWwindow* window, const float deltaTime);
};
