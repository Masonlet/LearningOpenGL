#include "camera.hpp"
#include "constants.hpp"

#include <cmath>
#include <cstdio>

Camera::Camera() :
  moveSpeed{ MOVE_SPEED }, mouseSpeed{ CAMERA_SPEED }, 
  pos{ INITIAL_POS },
  front{ INITIAL_TARGET },
  up{ WORLD_UP }, 
  yaw{ DEFAULT_YAW }, pitch{ DEFAULT_PITCH }, 
  lastX{ 0 }, lastY{ 0 }, 
  paused{ false } {
  front = front.normalized();
}

Vec3 Camera::GetRight() const {
  Vec3 right = front.cross(up);

  if (right.length() < 0.00001f)
    return { 1.0f, 0.0f, 0.0f };

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

Mat4 Camera::Perspective(const float aspect) const {
  const float tanHalfFov = tanf(radians(DEFAULT_FOV) / 2.0f);
  const float zRange = FAR_PLANE - NEAR_PLANE;

  Mat4 projection{};

  projection.data[0] = 1.0f / (aspect * tanHalfFov);
  projection.data[5] = 1.0f / tanHalfFov;
  projection.data[10] = -(FAR_PLANE + NEAR_PLANE) / zRange;
  projection.data[11] = -1.0f;
  projection.data[14] = -(2.0f * FAR_PLANE * NEAR_PLANE) / zRange;
  projection.data[15] = 0.0f;

  return projection;
};

void Camera::print(){
  fprintf(stdout, "Pos: %f:%f:%f\n", pos.x, pos.y, pos.z);
  fprintf(stdout, "Front: %f:%f:%f\n", front.x, front.y, front.z);
  fprintf(stdout, "Pitch: %f\n", pitch);
  fprintf(stderr, "\n");
}


