#pragma once

#include "scene/sceneManager.hpp"
#include "math/constants.hpp"

bool createSquareGrid(MeshManager* meshManager, unsigned int shaderProgramID, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rot = DEFAULT_ROTATION, const Vec2& size = DEFAULT_SIZE_2D);
bool createCubeGrid(MeshManager* meshManage, unsigned int shaderProgramIDr, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rot = DEFAULT_ROTATION, const Vec3& size = DEFAULT_SIZE_3D);

bool createMeshGridFromPath(MeshManager* meshManager, unsigned int shaderProgramID, const std::string& baseName, const std::string& path, int startIndex, int count, const Vec2& spacing, const Vec3& rot = DEFAULT_ROTATION, const Vec3& size = DEFAULT_SCALE, bool hasNormals = false);
