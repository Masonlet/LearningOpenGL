#include <glad/glad.h>
#include "graphics/meshManager.hpp"
#include "starletparsers/utils/log.hpp"

MeshManager::~MeshManager() {
  for (std::map<std::string, Mesh>::iterator it = pathToMeshes.begin(); it != pathToMeshes.end(); ++it) {
    Mesh& mesh = it->second;

    if (glIsVertexArray(mesh.VAOID))     glDeleteVertexArrays(1, &mesh.VAOID);
    if (glIsBuffer(mesh.VertexBufferID)) glDeleteBuffers(1, &mesh.VertexBufferID);
    if (glIsBuffer(mesh.IndexBufferID))  glDeleteBuffers(1, &mesh.IndexBufferID);
  }

  pathToMeshes.clear();
}

bool MeshManager::addMesh(const std::string& path) {
  if (findMesh(path)) return true;

  Mesh mesh;
  if(!loader.loadMesh(path, mesh)) 
    return error("MeshManager", "addMesh", "Could not load mesh from " + path);

  if (!loader.uploadMesh(mesh))
    return error("MeshManager", "addMesh", "Could not upload mesh from: " + path);

  this->pathToMeshes[path] = std::move(mesh);
  return true;
}
bool MeshManager::addMesh(const std::string& path, Mesh& mesh) {
  if (findMesh(path)) return true;
  if (mesh.empty()) return error("MeshManager", "addMesh", "Trying to add an empty mesh");
  
  this->pathToMeshes[path] = std::move(mesh);
  return true;
}

bool MeshManager::findMesh(const std::string& name) const {
	return pathToMeshes.find(name) != pathToMeshes.end();
}
bool MeshManager::getMesh(const std::string& name, Mesh*& data) {
  std::map<std::string, Mesh>::iterator it = pathToMeshes.find(name);
  if (it == pathToMeshes.end()) return false;
  data = &it->second;
  return true;
}