#include "input/cameraController.hpp"
#include "input/inputManager.hpp"
#include "starletmath/constants.hpp"
#include <cmath>

void FreeCameraController::update(Camera& camera, const InputManager& input, float deltaTime) {
  const float s = camera.moveSpeed * deltaTime;
  if (input.isKeyDown(GLFW_KEY_W)) camera.pos += camera.front * s;
  if (input.isKeyDown(GLFW_KEY_A)) camera.pos -= camera.GetRight() * s;
  if (input.isKeyDown(GLFW_KEY_S)) camera.pos -= camera.front * s;
  if (input.isKeyDown(GLFW_KEY_D)) camera.pos += camera.GetRight() * s;
  if (input.isKeyDown(GLFW_KEY_SPACE))        camera.pos += camera.up * s;
  if (input.isKeyDown(GLFW_KEY_LEFT_CONTROL)) camera.pos -= camera.up * s;

  if (!input.isCursorLocked()) return;

  camera.yaw += input.getMouseDelta().x * camera.mouseSpeed;
  camera.pitch += input.getMouseDelta().y * camera.mouseSpeed;

  if (camera.pitch > 89.0f)  camera.pitch = 89.0f;
  if (camera.pitch < -89.0f) camera.pitch = -89.0f;

  camera.front.x = cos(radians(camera.yaw)) * cos(radians(camera.pitch));
  camera.front.y = sin(radians(camera.pitch));
  camera.front.z = sin(radians(camera.yaw)) * cos(radians(camera.pitch));
  camera.front = camera.front.normalized();
  camera.up = camera.GetRight().cross(camera.front).normalized();
}

void FreeCameraController::adjustFov(Camera& data, const float delta) {
  data.fov += delta;
  if (data.fov < 1.0f)   data.fov = 1.0f;
  if (data.fov > 120.0f) data.fov = 120.0f;
}
void FreeCameraController::print(const Camera& data) const {
  printf("\nPos: %f:%f:%f\n", data.pos.x, data.pos.y, data.pos.z);
  printf("Front: %f:%f:%f\n", data.front.x, data.front.y, data.front.z);
  printf("Pitch: %f\n\n", data.pitch);
}


