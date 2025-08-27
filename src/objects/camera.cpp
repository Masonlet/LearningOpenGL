#include "objects/camera.hpp"
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











