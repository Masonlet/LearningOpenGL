#pragma once

#include "core/sceneLoader.hpp"
#include "math/constants.hpp"

bool createSquareGrid(SceneLoader& loader, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation = DEFAULT_ROTATION, const Vec2& size = DEFAULT_SIZE_2D);
bool createCubeGrid(SceneLoader& loader, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation = DEFAULT_ROTATION, const Vec3& size = DEFAULT_SIZE_3D);

bool createMeshGridFromPath(SceneLoader& loader, const std::string& baseName, const std::string& path, int startIndex, int count, const Vec2& spacing, const Vec3& rotation = DEFAULT_ROTATION, const Vec3& scale = DEFAULT_SCALE, bool hasNormals = false);
