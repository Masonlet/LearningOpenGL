#pragma once

#include "model.hpp"

constexpr Vec2 defaultSize2D{1.0f, 1.0f};
constexpr Vec3 defaultSize3D{1.0f, 1.0f, 1.0f};
constexpr Vec3 defaultRotation{0.0f, 0.0f, 0.0f};
constexpr Vec3 defaultScale{1.0f, 1.0f, 1.0f};

// Primitives
bool createTriangle(Mesh& mesh, const Vec2& size = defaultSize2D);

bool createSquare(Mesh& mesh, const Vec2& size = defaultSize2D);
bool createSquareGrid(Model* models, size_t startIndex, int count, const Vec2& spacing, const Vec3& rotation = defaultRotation, const Vec2& size = defaultSize2D);

bool createCube(Mesh& mesh, const Vec3& size = defaultSize3D);
bool createCubeGrid(Model* models, size_t startIndex, int count, const Vec2& spacing, const Vec3& rotation = defaultRotation, const Vec3& size = defaultSize3D);

// Meshes
bool createMeshPath(Mesh* mesh, const char* path, const char* type, const Vec3& scale = defaultScale, const bool hasNormals = false);
bool createModelGrid(Model* models, const char* path, size_t startIndex, int count, const Vec2& spacing, const Vec3& rotation = defaultRotation, const Vec3& scale = defaultScale, const bool hasNormals = false);