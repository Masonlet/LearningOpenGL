#pragma once

#include "math/constants.hpp"
#include "graphics/vaoManager.hpp"
#include "core/engine.hpp"

bool createTriangle(VAOManager* vaoManager, const std::string& name, unsigned int shaderID, const Vec2& size = DEFAULT_SIZE_2D, const Vec4& vertexColour = {1.0f, 1.0f, 1.0f, 1.0f});
bool createSquare(VAOManager* vaoManager, const std::string& name, unsigned int shaderID, const Vec2& size = DEFAULT_SIZE_2D);

bool fillCubeMeshData(ModelDrawInfo& info, const std::string& name, const Vec3& size);
bool createCube(VAOManager* vaoManager, const std::string& name, const Vec3& size = DEFAULT_SIZE_3D);