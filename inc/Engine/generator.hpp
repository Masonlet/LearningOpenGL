#pragma once

#include "model.hpp"

constexpr float defaultSize{1};

// Primitives
bool createTriangle(Mesh& mesh, const float  width = defaultSize, const float  height = defaultSize);

bool createSquare(Mesh& mesh, const float  width = defaultSize, const float  height = defaultSize);
bool createSquareGrid(Model* models, int startIndex, int count, float spacing, const Vec3& rotation = {0.0f, 0.0f, 0.0f}, const float width = defaultSize, const float height = defaultSize);

bool createCube(Mesh& mesh, const float  width = defaultSize, const float  height = defaultSize, const float depth = defaultSize);
bool createCubeGrid(Model* models, int startIndex, int count, float spacing, const Vec3& rotation = {0.0f, 0.0f, 0.0f}, const float width = defaultSize, const float height = defaultSize, const float depth = defaultSize);

// Meshes
bool createMeshPath(Mesh* mesh, const char* path, const char* type, const bool hasNormals);
bool createModelGrid(Model* models, const char* path, int startIndex, int count, float spacing, const bool hasNormals, const Vec3& rotation = {0.0f, 0.0f, 0.0f});