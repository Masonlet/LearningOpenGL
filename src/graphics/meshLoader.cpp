#include "graphics/mesh.hpp"
#include "graphics/meshLoader.hpp"
#include <glad/glad.h>
#include "parsers/meshes/plyParser.hpp"
#include "utils/log.hpp"

bool MeshLoader::loadMesh(const std::string& path, Mesh& mesh) {
  if (!mesh.empty()) return error("MeshLoader", "loadMesh", "Loading non-empty mesh: " + path);
    if (!parsePlyMesh(path, mesh)) 
      return error("MeshLoader", "loadMesh", "Failed to parse mesh file: " + path);
  
  return true;
}

bool MeshLoader::uploadMesh(Mesh& mesh) {
  if (mesh.empty()) return error("MeshLoader", "uploadMesh", "Invalid mesh data");

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

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) return error("MeshManager", "UploadToGPU", "OpenGL error " + std::to_string(err));

  delete[] mesh.vertices;
  mesh.vertices = nullptr;
  delete[] mesh.indices;
  mesh.indices = nullptr;
  return true;
}