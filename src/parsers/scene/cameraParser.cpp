#pragma once

#include "scene/objects/camera.hpp"
#include "parsers/scene/cameraParser.hpp"
#include "parsers/parserFunctions.hpp"

bool parseCamera(const unsigned char*& p, Camera& out) {
  PARSE_OR(return false, parseBool, out.enabled, "camera enabled");
  PARSE_STRING_OR(return false, p, out.name, 64, "camera name");
  PARSE_OR(return false, parseVec3, out.pos, "camera position");
  PARSE_OR(return false, parseFloat, out.yaw, "camera yaw");
  PARSE_OR(return false, parseFloat, out.pitch, "camera pitch");
  PARSE_OR(return false, parseFloat, out.fov, "camera fov");
  PARSE_OR(return false, parseFloat, out.nearPlane, "camera near plane");
  PARSE_OR(return false, parseFloat, out.farPlane, "camera far plane");
  PARSE_OR(return false, parseFloat, out.moveSpeed, "camera speed");
  return true;
}