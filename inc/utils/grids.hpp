#pragma once

#include "scene/sceneManager.hpp"
#include "math/constants.hpp"

bool createSquareGrid(SceneManager& loader, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation = DEFAULT_ROTATION, const Vec2& size = DEFAULT_SIZE_2D);
bool createCubeGrid(SceneManager& loader, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation = DEFAULT_ROTATION, const Vec3& size = DEFAULT_SIZE_3D);

bool createMeshGridFromPath(SceneManager& loader, const std::string& baseName, const std::string& path, int startIndex, int count, const Vec2& spacing, const Vec3& rotation = DEFAULT_ROTATION, const Vec3& scale = DEFAULT_SCALE, bool hasNormals = false);
