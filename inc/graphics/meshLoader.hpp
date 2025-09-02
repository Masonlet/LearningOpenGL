#pragma once

struct Mesh;
#include <string>

struct MeshLoader {
	bool loadMesh(const std::string& path, Mesh& mesh);
	bool uploadMesh(Mesh& mesh);
};	