#include "controllers/cameraController.hpp"
#include "math/constants.hpp"
#include <cmath>

void FreeCameraController::update(Camera& camera, const InputManager& input, float deltaTime) {
  const float s = camera.moveSpeed * deltaTime;
  if (input.IsKeyDown(GLFW_KEY_W)) camera.pos += camera.front * s;
  if (input.IsKeyDown(GLFW_KEY_A)) camera.pos -= camera.GetRight() * s;
  if (input.IsKeyDown(GLFW_KEY_S)) camera.pos -= camera.front * s;
  if (input.IsKeyDown(GLFW_KEY_D)) camera.pos += camera.GetRight() * s;
  if (input.IsKeyDown(GLFW_KEY_SPACE))        camera.pos += camera.up * s;
  if (input.IsKeyDown(GLFW_KEY_LEFT_CONTROL)) camera.pos -= camera.up * s;

  if (!input.IsCursorLocked()) return;

  camera.yaw += input.GetMouseDelta().x * camera.mouseSpeed;
  camera.pitch += input.GetMouseDelta().y * camera.mouseSpeed;

  if (camera.pitch > 89.0f)  camera.pitch = 89.0f;
  if (camera.pitch < -89.0f) camera.pitch = -89.0f;

  camera.front.x = cos(radians(camera.yaw)) * cos(radians(camera.pitch));
  camera.front.y = sin(radians(camera.pitch));
  camera.front.z = sin(radians(camera.yaw)) * cos(radians(camera.pitch));
  camera.front = camera.front.normalized();
  camera.up = camera.GetRight().cross(camera.front).normalized();
}
