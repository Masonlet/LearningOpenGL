#pragma once

#include "scene/objects/model.hpp"
#include "parsers/scene/modelParser.hpp"
#include "parsers/parserFunctions.hpp"

bool parseModel(const unsigned char*& p, Model& out) {
  PARSE_OR(return false, parseBool, out.isVisible, "model enabled");
  PARSE_OR(return false, parseBool, out.isLighted, "model lighting");
  PARSE_STRING_OR(return false, p, out.name, 64, "model name");
  PARSE_STRING_OR(return false, p, out.meshPath, 128, "model path");
  Vec3 temp;
  PARSE_OR(return false, parseVec3, temp, "model position");
  out.transform.pos = { temp, 1.0f };
  PARSE_OR(return false, parseVec3, out.transform.rot, "model rotation");
  PARSE_OR(return false, parseVec3, out.transform.size, "model scale");
  if (!parseColour(p, out.colour, out.colourMode)) return false;
  PARSE_OR(return false, parseVec4, out.specular, "model specular");
  return true;
}
