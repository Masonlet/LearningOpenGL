#pragma once

#include "math/constants.hpp"
#include "graphics/meshManager.hpp"

bool createTriangle(MeshManager* meshManager, const std::string& name, unsigned int shaderID, const Vec2& size = DEFAULT_SIZE_2D, const Vec4& vertexColour = {1.0f, 1.0f, 1.0f, 1.0f});
bool createSquare(MeshManager* meshManager, const std::string& name, unsigned int shaderID, const Vec2& size = DEFAULT_SIZE_2D);
bool createCube(MeshManager* meshManager, const std::string& name, const Vec3& size = DEFAULT_SIZE_3D);