#include <glad/glad.h>
#include "graphics/meshManager.hpp"
#include "utils/fileParser.hpp"
#include "models/plyParser.hpp"

bool MeshManager::loadMeshFile(const std::string& path, MeshData& mesh, unsigned int shaderID) {
	mesh.path = path;
	mesh.hasNormals = false;
	mesh.hasColours = false;
	mesh.numVertices = 0;
	mesh.numIndices = 0;

  unsigned char* src{ nullptr };
  size_t size;
  if (!loadBinaryFile(src, size, "assets/models/" + path))
    return false;

  if (!parsePlyMesh(src, size, mesh))
    return false;

  if (!mesh.vertices || !mesh.indices || mesh.numVertices == 0 || mesh.numIndices == 0) {
    fprintf(stderr, "[LoadMeshFromFile ERROR] Invalid mesh data: %s\n", path.c_str());
    return false;
  }

	return UploadMeshToGPU(mesh, shaderID);
}
bool MeshManager::loadMeshPrimitive(MeshData& mesh, unsigned int shaderID) {
  if(!mesh.vertices || !mesh.indices || mesh.numVertices == 0 || mesh.numIndices == 0) {
    fprintf(stderr, "[LoadMeshPrimitive ERROR] Invalid mesh data\n");
    return false;
	}

	return UploadMeshToGPU(mesh, shaderID);
}

bool MeshManager::findMesh(const std::string& path, const MeshData*& mesh) const {
	std::map<std::string, MeshData>::const_iterator it = nameToMeshes.find(path);
	if (it == nameToMeshes.end()) return false;

	mesh = &it->second;
	return true;
}

bool MeshManager::UploadMeshToGPU(MeshData& mesh, unsigned int shaderID) {
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

  //Set the vertex attributes
  int vpos_location = glGetAttribLocation(shaderID, "vPos");
  if (vpos_location != -1) {
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
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
    glVertexAttribPointer(vTextCoords_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
  }

  // Now that all the parts are set up, set the VAO to zero
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
		printf("[UploadToGPU ERROR] OpenGL error \"%u\"after uploading mesh %s\n", err, mesh.path.c_str());
    return false;
  }

  delete[] mesh.vertices;
  mesh.vertices = nullptr;

  delete[] mesh.indices;
  mesh.indices = nullptr;

  this->nameToMeshes[mesh.path] = std::move(mesh);
  return true;
}

void MeshManager::Shutdown() {
  for (std::map<std::string, MeshData>::iterator it = nameToMeshes.begin(); it != nameToMeshes.end(); ++it) {
    MeshData& mesh = it->second;

    if (glIsVertexArray(mesh.VAOID))     glDeleteVertexArrays(1, &mesh.VAOID);
    if (glIsBuffer(mesh.VertexBufferID)) glDeleteBuffers(1, &mesh.VertexBufferID);
    if (glIsBuffer(mesh.IndexBufferID))  glDeleteBuffers(1, &mesh.IndexBufferID);
  }

  nameToMeshes.clear();
}