#pragma once

#include "graphics/vaoManager.hpp"
#include "math/vec3.hpp"
#include <string>

bool createMeshFromPath(VAOManager& vaoManager, const std::string& name, const std::string& path, const Vec3& scale, bool hasNormals);