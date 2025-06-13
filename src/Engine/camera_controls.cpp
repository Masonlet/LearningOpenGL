#include "camera.hpp"
#include "constants.hpp"

#include <cmath>

void Camera::ProcessInputs(GLFWwindow* window, Vec3& pos, Vec3& rot, Vec3& scale, const float deltaTime) {
  double xpos{ 0.0 }, ypos{ 0.0 };
  glfwGetCursorPos(window, &xpos, &ypos);

  ProcessKeyboard(window, deltaTime);
  ProcessMouse(window, xpos, ypos);
}

void Camera::ProcessKeyboard(GLFWwindow* window, const float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) MoveForward(deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) MoveLeft(deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) MoveBackward(deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) MoveRight(deltaTime);

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) MoveUp(deltaTime);
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) MoveDown(deltaTime);
}

void Camera::ProcessMouse(GLFWwindow* window, const double xpos, const double ypos) {
  if (glfwGetInputMode(window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
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
    right = { 1.0f, 0.0f, 0.0f };

  up = right.cross(front).normalized();
}

