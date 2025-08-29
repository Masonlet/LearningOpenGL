#include "objects/camera.hpp"
#include "starletmath/constants.hpp"

#include <cmath>

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











