#include "core/camera.hpp"
#include "math/constants.hpp"

#include <cmath>

Vec3 Camera::GetRight() const {
  Vec3 right = front.cross(WORLD_UP);
  if (right.length() < 0.00001f) return { 1.0f, 0.0f, 0.0f };
  else                           return right.normalized();
}

Mat4 Camera::LookAt() const {
  const Vec3 forward = front.normalized();
  const Vec3 right   = GetRight();
  const Vec3 camUp   = right.cross(forward);

  Mat4 view{};
  view.models[0] = right.x;
  view.models[1] = camUp.x;
  view.models[2] = -forward.x;
  view.models[3] = 0.0f;

  view.models[4] = right.y;
  view.models[5] = camUp.y;
  view.models[6] = -forward.y;
  view.models[7] = 0.0f;

  view.models[8] = right.z;
  view.models[9] = camUp.z;
  view.models[10] = -forward.z;
  view.models[11] = 0.0f;

  view.models[12] = -right.dot(pos);
  view.models[13] = -camUp.dot(pos);
  view.models[14] = forward.dot(pos);
  view.models[15] = 1.0f;
  return view;
}

Mat4 Camera::Perspective(const float aspect) const {
  const float tanHalfFov = tanf(radians(fov) / 2.0f);

  Mat4 projection{};
  projection.models[0]  =   1.0f / (aspect * tanHalfFov);
  projection.models[5]  =   1.0f / tanHalfFov;
  projection.models[10] = -(farPlane + nearPlane) / (farPlane - nearPlane);
  projection.models[11] =  -1.0f;
  projection.models[14] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
  projection.models[15] =   0.0f;
  return projection;
};

void Camera::print() const {
  printf("\nPos: %f:%f:%f\n", pos.x, pos.y, pos.z);
  printf("Front: %f:%f:%f\n", front.x, front.y, front.z);
  printf("Pitch: %f\n\n", pitch);
}

void Camera::setFov(const float delta) {
  fov += delta;
  if (fov < 1.0f)   fov = 1.0f;
  if (fov > 120.0f) fov = 120.0f;
}

void Camera::processInputs(InputManager* input, float deltaTime) {
  switch (type) {
  case 0: updateFreeCam(input, deltaTime);    break;
  case 1: updateDungeonCam(input, deltaTime); break;
  case 2: updateModernCam(input, deltaTime);  break;
  }
}

void Camera::updateFreeCam(InputManager* input, float deltaTime) {
  processKeyboard(input, deltaTime);
  processMouse(input);
}
void Camera::updateDungeonCam(InputManager* input, float deltaTime) {
  pitch = 0.0f;

  if (input->IsKeyPressed(GLFW_KEY_A)) yaw -= 90.0f;
  if (input->IsKeyPressed(GLFW_KEY_D)) yaw += 90.0f;
  yaw = std::round(yaw / 90.0f) * 90.0f;
  yaw = fmod(yaw + 360.0f, 360.0f);

  front = Vec3{ cos(radians(yaw)), 0.0f, sin(radians(yaw)) }.normalized();
  if (input->IsKeyPressed(GLFW_KEY_W)) pos += front * moveDistance;
  if (input->IsKeyPressed(GLFW_KEY_S)) pos -= front * moveDistance;

  Vec3 right = GetRight();
  up = right.cross(front).normalized();
}
void Camera::updateModernCam(InputManager* input, float deltaTime) {
  processMouse(input);

  Vec3 flatFront = front;
  flatFront.y = 0.0f;
  flatFront = flatFront.normalized();

  Vec3 right = flatFront.cross(WORLD_UP).normalized();

  Vec3 moveDir{ 0.0f };
  if (input->IsKeyDown(GLFW_KEY_W)) moveDir += flatFront;
  if (input->IsKeyDown(GLFW_KEY_S)) moveDir -= flatFront;
  if (input->IsKeyDown(GLFW_KEY_D)) moveDir += right;
  if (input->IsKeyDown(GLFW_KEY_A)) moveDir -= right;

  if (moveDir.length() > 0.0f) {
    moveDir = moveDir.normalized();
    pos += moveDir * moveSpeed * deltaTime;
  }
}

void Camera::processKeyboard(InputManager* input, const float deltaTime) {
  if (input->IsKeyDown(GLFW_KEY_W)) moveForward(deltaTime);
  if (input->IsKeyDown(GLFW_KEY_A)) moveLeft(deltaTime);
  if (input->IsKeyDown(GLFW_KEY_S)) moveBackward(deltaTime);
  if (input->IsKeyDown(GLFW_KEY_D)) moveRight(deltaTime);
  if (input->IsKeyDown(GLFW_KEY_SPACE))        moveUp(deltaTime);
  if (input->IsKeyDown(GLFW_KEY_LEFT_CONTROL)) moveDown(deltaTime);
}

void Camera::processMouse(InputManager* input) {
  if (!input->IsCursorLocked()) return;

  Vec2 delta = input->GetMouseDelta();
  float xoffset = delta.x * mouseSpeed;
  float yoffset = delta.y * mouseSpeed;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)  pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;

  front.x = cos(radians(yaw)) * cos(radians(pitch));
  front.y = sin(radians(pitch));
  front.z = sin(radians(yaw)) * cos(radians(pitch));
  front = front.normalized();

  Vec3 right = GetRight();
  up = right.cross(front).normalized();
}



