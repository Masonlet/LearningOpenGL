#pragma once

#include "starletparsers/meshes/mesh.hpp"
#include "starletmath/constants.hpp"
#include "meshLoader.hpp"
#include <string>	
#include <map>

class MeshManager {
public:
	MeshManager() = default;
	~MeshManager();

	bool addMesh(const std::string& path);
	bool addMesh(const std::string& path, Mesh& mesh);

	bool createTriangle(const std::string& name, unsigned int shaderID, const Vec2& size = DEFAULT_SIZE_2D, const Vec4& vertexColour = { 1.0f, 1.0f, 1.0f, 1.0f });
	bool createSquare(const std::string& name, unsigned int shaderID, const Vec2& size = DEFAULT_SIZE_2D);
	bool createCube(const std::string& name, const Vec3& size);

	bool findMesh(const std::string& path) const;
	bool getMesh(const std::string& path, Mesh*& dataOut);

private:
	MeshLoader loader;
	std::map<std::string, Mesh> pathToMeshes;
};