#include <glad/glad.h>
#include "graphics/meshManager.hpp"
#include "utils/log.hpp"

MeshManager::~MeshManager() {
  for (std::map<std::string, Mesh>::iterator it = nameToMeshes.begin(); it != nameToMeshes.end(); ++it) {
    Mesh& mesh = it->second;

    if (glIsVertexArray(mesh.VAOID))     glDeleteVertexArrays(1, &mesh.VAOID);
    if (glIsBuffer(mesh.VertexBufferID)) glDeleteBuffers(1, &mesh.VertexBufferID);
    if (glIsBuffer(mesh.IndexBufferID))  glDeleteBuffers(1, &mesh.IndexBufferID);
  }

  nameToMeshes.clear();
}

bool MeshManager::findMesh(const std::string& path) const {
	return nameToMeshes.find(path) != nameToMeshes.end();
}
bool MeshManager::getMesh(const std::string& name, Mesh*& data) {
  std::map<std::string, Mesh>::iterator it = nameToMeshes.find(name);
  if (it == nameToMeshes.end()) return false;
  data = &it->second;
  return true;
}

bool MeshManager::uploadMeshToGPU(const std::string& path, Mesh& mesh, unsigned int shaderID) {
  if (mesh.empty())   return error("MeshManager", "uploadMeshToGPU", "Invalid mesh data");
  if (findMesh(path)) return error("MeshManager", "uploadMeshToGPU", "Mesh data exists already");

  //Create a VAO (Vertex Array Object), which will keep track of all the 'state' needed to draw from this buffer
  glGenVertexArrays(1, &(mesh.VAOID)); //Ask OpenGL for a new buffer ID
  glBindVertexArray(mesh.VAOID);       //Bind the buffer: aka "make this the 'current' VAO buffer

  //Now ANY state that is related to vertex or index buffer and vertex attribute layout, is stored in the 'state' of the VAO
  glGenBuffers(1, &(mesh.VertexBufferID));
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh.numVertices, mesh.vertices, GL_STATIC_DRAW);

  //Copy the index buffer into the video card to create an index buffer
  glGenBuffers(1, &(mesh.IndexBufferID));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.IndexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.numIndices, mesh.indices, GL_STATIC_DRAW);

  int vpos_location = glGetAttribLocation(shaderID, "vPos");
  if (vpos_location != -1) {
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location,  4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  }
  int vnorm_location = glGetAttribLocation(shaderID, "vNorm");
  if (vnorm_location != -1) {
    glEnableVertexAttribArray(vnorm_location);
    glVertexAttribPointer(vnorm_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
  }
  int vcol_location = glGetAttribLocation(shaderID, "vCol");
  if (vcol_location != -1) {
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));
  }

  int vTextCoords_location = glGetAttribLocation(shaderID, "vTextCoords");
  if (vTextCoords_location != -1) {
    glEnableVertexAttribArray(vTextCoords_location);
    glVertexAttribPointer(vTextCoords_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
  }
    
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  GLenum err = glGetError();
	if (err != GL_NO_ERROR) return error("MeshManager", "UploadToGPU", "OpenGL error " + std::to_string(err) + "after uploading mesh " + path);

  delete[] mesh.vertices;
  mesh.vertices = nullptr;
  delete[] mesh.indices;
  mesh.indices = nullptr;

  this->nameToMeshes[path] = std::move(mesh);
  return true;
}