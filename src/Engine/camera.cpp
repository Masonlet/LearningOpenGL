#include "camera.hpp"
#include <cmath>
#include <cstdio>

Camera::Camera() : moveSpeed{MOVE_SPEED}, mouseSpeed{CAMERA_SPEED}, pos{INITIAL_POS},
					front{INITIAL_TARGET}, up{WORLD_UP}, yaw{DEFAULT_YAW},
	pitch{DEFAULT_PITCH}, lastX{0}, lastY{0}, paused{false} {
	front = front.normalized();
}

Vec3 Camera::GetRight() const {
	Vec3 right = front.cross(up);

	if (right.length() < 0.00001f)
		return {1.0f, 0.0f, 0.0f};

	return right.normalized();
}
Mat4 Camera::LookAt() const {
	Mat4 view{};

	const Vec3 forward = front.normalized();
	const Vec3 right = GetRight();
	const Vec3 camUp = right.cross(forward);

	view.data[0] = right.x;
	view.data[1] = camUp.x;
	view.data[2] = -forward.x;
	view.data[3] = 0.0f;

	view.data[4] = right.y;
	view.data[5] = camUp.y;
	view.data[6] = -forward.y;
	view.data[7] = 0.0f;

	view.data[8] = right.z;
	view.data[9] = camUp.z;
	view.data[10] = -forward.z;
	view.data[11] = 0.0f;

	view.data[12] = -right.dot(pos);
	view.data[13] = -camUp.dot(pos);
	view.data[14] = forward.dot(pos);
	view.data[15] = 1.0f;

	return view;
}

//void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
//void gluPerspective(GLdouble fov, GLdouble aspect, GLdouble near, GLdouble far);
Mat4 Camera::Perspective(const float aspect) const {
	const float tanHalfFov = tanf(radians(DEFAULT_FOV) / 2.0f);
	const float zRange = FAR_PLANE - NEAR_PLANE;

	Mat4 projection{};

	projection.data[0]  = 1.0f / (aspect * tanHalfFov); 
    projection.data[5]  = 1.0f / tanHalfFov;           
    projection.data[10] = -(FAR_PLANE + NEAR_PLANE) / zRange;           
    projection.data[11] = -1.0f;                                     
    projection.data[14] = -(2.0f * FAR_PLANE * NEAR_PLANE) / zRange; 
	projection.data[15] = 0.0f;

	return projection;
};
//glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near, GLdouble far);
//gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
Mat4 Camera::Orthographic() const {
	const float size = 10.0f;
	const float left = -size;
	const float right = size;
	const float bottom = -size;
	const float top = size;

	Mat4 orthographic;

	orthographic.data[0]  = 2.0f / (right - left);
	orthographic.data[5]  = 2.0f / (top - bottom);
	orthographic.data[10] = -2.0f / (FAR_PLANE - NEAR_PLANE);
	orthographic.data[12] = -(right + left) / (right - left);
	orthographic.data[13] = -(top + bottom) / (top - bottom);
	orthographic.data[14] = -(FAR_PLANE + NEAR_PLANE) / (FAR_PLANE - NEAR_PLANE);
	orthographic.data[15] = 1.0f;	

	return orthographic;
}

void Camera::MoveForward(const float deltaTime) {
	pos += front * (deltaTime * moveSpeed);
}
void Camera::MoveBackward(const float deltaTime) {
	pos -= front * (deltaTime * moveSpeed);
}
void Camera::MoveLeft(const float deltaTime) {
	pos -= GetRight() * (deltaTime * moveSpeed);
}
void Camera::MoveRight(const float deltaTime) {
	pos += GetRight() * (deltaTime * moveSpeed);
}
void Camera::MoveUp(const float deltaTime) {
	pos += up * (deltaTime * moveSpeed);
}
void Camera::MoveDown(const float deltaTime) {
	pos -= up * (deltaTime * moveSpeed);
}

void Camera::ProcessKeyboard(GLFWwindow* window, Transform& translation, const float deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) MoveForward(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) MoveLeft(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) MoveBackward(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) MoveRight(deltaTime);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) MoveUp(deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) MoveDown(deltaTime);

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) translation.move({0.0f, 0.0f, deltaTime});
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) translation.move({0.0f, 0.0f, -deltaTime});
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) translation.move({-deltaTime, 0.0f, 0.0f});
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) translation.move({deltaTime, 0.0f, 0.0f});

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) translation.scale({1.001f, 1.001f, 1.001f});
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) translation.scale({0.999f, 0.999f, 0.999f});

	const float rotationSpeed = 45.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) translation.rotate({ rotationSpeed, 0.0f, 0.0f }); 
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) translation.rotate({ -rotationSpeed, 0.0f, 0.0f }); 
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) translation.rotate({ 0.0f, rotationSpeed, 0.0f }); 
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) translation.rotate({ 0.0f, -rotationSpeed, 0.0f }); 
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) translation.rotate({ 0.0f, 0.0f, rotationSpeed }); 
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) translation.rotate({ 0.0f, 0.0f, -rotationSpeed }); 
}
void Camera::ProcessMouse(GLFWwindow* window, RenderMode renderMode, const double xpos, const double ypos) {
	if (renderMode == RenderMode::basic2D || glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
		return;

	float xoffset = static_cast<float>(xpos - lastX);
	float yoffset = static_cast<float>(lastY - ypos);

	lastX = static_cast<float>(xpos);
	lastY = static_cast<float>(ypos);

	xoffset *= mouseSpeed;
	yoffset *= mouseSpeed;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	front.x = cos(radians(yaw)) * cos(radians(pitch));
	front.y = sin(radians(pitch));
	front.z = sin(radians(yaw)) * cos(radians(pitch));
	front = front.normalized();

	Vec3 right = front.cross(WORLD_UP).normalized();
	if (right.length() < 1e-6f)
		right = {1.0f, 0.0f, 0.0f};

	up = right.cross(front).normalized();
}

void Camera::ProcessInputs(GLFWwindow* window, Transform& translation, RenderMode renderMode, const float deltaTime) {
	double xpos{0.0}, ypos{0.0};
	glfwGetCursorPos(window, &xpos, &ypos);

	/*fprintf(stdout, "Pos: %f:%f:%f\n", pos.x, pos.y, pos.z);
	fprintf(stdout, "Front: %f:%f:%f\n", front.x, front.y, front.z);
	fprintf(stdout, "Pitch: %f\n", pitch);
	fprintf(stderr, "\n");*/

	ProcessKeyboard(window, translation, deltaTime);
	ProcessMouse(window, renderMode, xpos, ypos);
}

