#pragma once

#include "scene/objects/light.hpp"
#include "parsers/scene/lightParser.hpp"
#include "parsers/parserFunctions.hpp"

bool parseLight(const unsigned char*& p, Light& out) {
  PARSE_OR(return false, parseBool, out.enabled, "light enabled");
  PARSE_STRING_OR(return false, p, out.name, 64, "light name");
  PARSE_OR(return false, parseLightType, out.type, "light type");
  PARSE_OR(return false, parseVec3, out.pos, "light position");
  PARSE_OR(return false, parseVec4, out.diffuse, "light diffuse");
  PARSE_OR(return false, parseVec4, out.attenuation, "light attenuation");
  PARSE_OR(return false, parseVec3, out.direction, "light direction");
  PARSE_OR(return false, parseVec2, out.param1, "light param1");
  return true;
}