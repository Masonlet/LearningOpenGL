#pragma once

#include "graphics/objects/mesh.hpp"
#include <string>

struct MeshLoader {
	bool loadMesh(const std::string& path, Mesh& mesh);
	bool uploadMesh(Mesh& mesh);
};	