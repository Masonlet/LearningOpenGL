#include "core/camera.hpp"
#include "math/constants.hpp"

Camera::Camera() :
  moveSpeed{ MOVE_SPEED }, mouseSpeed{ CAMERA_SPEED }, 
  pos{ INITIAL_POS }, front{ INITIAL_TARGET }, up{ WORLD_UP }, 
  yaw{ DEFAULT_YAW }, pitch{ DEFAULT_PITCH }, 
  lastX{ 0 }, lastY{ 0 }, 
  fov{ DEFAULT_FOV }, nearPlane{NEAR_PLANE}, farPlane{FAR_PLANE},
  paused{ false } {
  front = front.normalized();
}

Vec3 Camera::GetRight() const {
  Vec3 right = front.cross(up);

  if (right.length() < 0.00001f) return { 1.0f, 0.0f, 0.0f };
  else return right.normalized();
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

Mat4 Camera::Perspective(const float aspect) const {
  const float tanHalfFov = tanf(radians(fov) / 2.0f);
  const float zRange = farPlane - nearPlane;

  Mat4 projection{};

  projection.data[0] = 1.0f / (aspect * tanHalfFov);
  projection.data[5] = 1.0f / tanHalfFov;
  projection.data[10] = -(farPlane + nearPlane) / zRange;
  projection.data[11] = -1.0f;
  projection.data[14] = -(2.0f * farPlane * nearPlane) / zRange;
  projection.data[15] = 0.0f;

  return projection;
};

void Camera::setFov(const float delta) {
  fov += delta;

  if (fov < 1.0f) fov = 1.0f;
  if (fov > 120.0f) fov = 120.0f;
}

void Camera::processInputs(InputManager* input, float deltaTime) {
  switch (type) {
  case 0: 
    updateFreeCam(input, deltaTime);
    break;
  case 1: 
    updateDungeonCam(input, deltaTime);
    break;
  case 2:
    updateModernCam(input, deltaTime);
    break;
  }
}

void Camera::updateFreeCam(InputManager* input, float deltaTime) {
  processKeyboard(input, deltaTime);
  processMouse(input);
}
void Camera::updateDungeonCam(InputManager* input, float deltaTime) {
  if (input->IsKeyPressed(GLFW_KEY_A)) yaw -= 90.0f;
  if (input->IsKeyPressed(GLFW_KEY_D)) yaw += 90.0f;

  yaw = std::round(yaw / 90.0f) * 90.0f;
  yaw = fmod(yaw + 360.0f, 360.0f);

  front = Vec3{ cos(radians(yaw)), 0.0f, sin(radians(yaw)) }.normalized();
  Vec3 right = front.cross(WORLD_UP).normalized();
  up = right.cross(front).normalized();

  if (input->IsKeyPressed(GLFW_KEY_W)) pos += front * moveDistance;
  if (input->IsKeyPressed(GLFW_KEY_S)) pos -= front * moveDistance;

  pitch = 0.0f;
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

  if (input->IsKeyDown(GLFW_KEY_SPACE)) moveUp(deltaTime);
  if (input->IsKeyDown(GLFW_KEY_LEFT_CONTROL)) moveDown(deltaTime);
}

void Camera::processMouse(InputManager* input) {
  if (!input->IsCursorLocked())
    return;

  Vec2 delta = input->GetMouseDelta();

  float xoffset = delta.x * mouseSpeed;
  float yoffset = delta.y * mouseSpeed;

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

void Camera::print() const {
  printf("\nPos: %f:%f:%f\n", pos.x, pos.y, pos.z);
  printf("Front: %f:%f:%f\n", front.x, front.y, front.z);
  printf("Pitch: %f\n\n", pitch);
}


