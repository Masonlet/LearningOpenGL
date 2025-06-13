#pragma once

#include "vec2.hpp"
#include "vec3.hpp"
#include "vaoManager.hpp"
#include "engine.hpp"

constexpr Vec2 defaultSize2D{1.0f, 1.0f};
constexpr Vec3 defaultSize3D{1.0f, 1.0f, 1.0f};
constexpr Vec3 defaultRotation{0.0f, 0.0f, 0.0f};
constexpr Vec3 defaultScale{1.0f, 1.0f, 1.0f};

// Primitives (GPU-uploaded via vaoManager)
bool createTriangle(VAOManager* vaoManager, const std::string& name, const Vec2& size = defaultSize2D, const Vec4& vertexColour = {1.0f, 1.0f, 1.0f, 1.0f});
bool createSquare(VAOManager* vaoManager, const std::string& name, const Vec2& size = defaultSize2D);
bool createCube(VAOManager* vaoManager, const std::string& name, const Vec3& size = defaultSize3D);

// Primitive grids (transformed instance data)
bool createSquareGrid(Engine& engine, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation = defaultRotation, const Vec2& size = defaultSize2D);
bool createCubeGrid(Engine& engine, const std::string& baseName, int startIndex, int count, const Vec2& spacing, const Vec3& rotation = defaultRotation, const Vec3& size = defaultSize3D);

// External Meshes
bool createMeshFromPath(VAOManager& vaoManager, const std::string& name, const std::string& path, const Vec3& scale = defaultScale, bool hasNormals = false);
bool createMeshGridFromPath(VAOManager& vaoManager, const std::string& baseName, const std::string& path, int startIndex, int count, const Vec2& spacing, const Vec3& rotation = defaultRotation, const Vec3& scale = defaultScale, bool hasNormals = false);
