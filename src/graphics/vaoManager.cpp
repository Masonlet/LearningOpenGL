#include <glad/glad.h>

#include "graphics/vaoManager.hpp"
#include "utils/parser.hpp"
#include "utils/plyParser.hpp"
#include "utils/files.hpp"

#include <sstream>

bool VAOManager::LoadPrimitiveIntoVAO(ModelDrawInfo& drawInfo, unsigned int shaderProgramID){
  if (!UploadToGPU(drawInfo, shaderProgramID)) {
    AppendTextToLastError("Failed to upload primitive to GPU", true);
    return false;
  }

  return true;
}
bool VAOManager::LoadModelIntoVAO(std::string fileName, ModelDrawInfo& drawInfo, unsigned int shaderProgramID) {
  drawInfo.meshPath = fileName;
  drawInfo.hasNormals = false;
  drawInfo.hasColours = false;
  drawInfo.numVertices = 0;
  drawInfo.numTriangles = 0;

  if (!this->LoadModelFromFile(fileName, drawInfo)) {
    this->AppendTextToLastError("Didnt load model", true);
    return false;
  }

  if (!drawInfo.vertices || !drawInfo.indices || drawInfo.numVertices == 0 || drawInfo.numIndices == 0) {
    this->AppendTextToLastError("Invalid model data — skipping GPU upload", true);
    return false;
  }

  return UploadToGPU(drawInfo, shaderProgramID);
}

bool VAOManager::FindDrawInfoByModelName(const std::string& fileName, const ModelDrawInfo*& drawInfo) {
  std::map<std::string, ModelDrawInfo>::iterator itDrawInfo = this->modelName_to_VAOID.find(fileName);
  if (itDrawInfo == this->modelName_to_VAOID.end()) return false;

  drawInfo = &itDrawInfo->second;
  return true;
}

bool VAOManager::LoadModelFromFile(const std::string& path, ModelDrawInfo& drawInfo) {
  unsigned char* src{ nullptr };
  size_t dummy;
  if (!loadBinaryFile(src, dummy, "assets/models/" + path)) {
    fprintf(stderr, "[LoadModelFromFile ERROR] Failed to load file: %s\n", path.c_str());
    return false;
  }

  const unsigned char* cursor = parsePlyHeader(src, drawInfo.numVertices,  drawInfo.numTriangles, drawInfo.hasNormals, drawInfo.hasColours);
  if(!cursor){
    fprintf(stderr, "[LoadModelFromFile ERROR] Failed to parse header or missing 'end_header'\n");
    delete[] drawInfo.indices;
    drawInfo.indices = nullptr;
    delete[] drawInfo.vertices;
    drawInfo.vertices = nullptr;
    delete[] src; 
    return false;
  }

  if (drawInfo.numVertices == 0 || drawInfo.numTriangles == 0) {
    fprintf(stderr, "[LoadModelFromFile ERROR] Header found but no vertices/triangles declared\n");
    delete[] drawInfo.indices;
    drawInfo.indices = nullptr;
    delete[] drawInfo.vertices;
    drawInfo.vertices = nullptr;
    delete[] src; 
    return false;
  }
  drawInfo.numIndices = drawInfo.numTriangles * 3;

  drawInfo.vertices = new Vertex[drawInfo.numVertices];
  cursor = parseVertices(drawInfo, cursor);
  if (!cursor) {
    fprintf(stderr, "[LoadModelFromFile ERROR] Vertex data input failed\n");
    delete[] drawInfo.indices;
    drawInfo.indices = nullptr;
    delete[] drawInfo.vertices;
    drawInfo.vertices = nullptr;
    delete[] src;
    return false;
  }

  drawInfo.indices = new unsigned int[drawInfo.numIndices];
  cursor = parseIndices(drawInfo, cursor);
  if (!cursor) {
    fprintf(stderr, "[LoadModelFromFile ERROR] Face data input failed\n");
    delete[] drawInfo.indices;
    drawInfo.indices = nullptr;
    delete[] drawInfo.vertices;
    drawInfo.vertices = nullptr;
    delete[] src;
    return false;
  }

  delete[] src;
  return true;
}

bool VAOManager::UploadToGPU(ModelDrawInfo& drawInfo, unsigned int shaderProgramID) {
  //Create a VAO (Vertex Array Object), which will keep track of all the 'state' needed to draw from this buffer
  glGenVertexArrays(1, &(drawInfo.VAOID)); //Ask OpenGL for a new buffer ID
  glBindVertexArray(drawInfo.VAOID);       //Bind the buffer: aka "make this the 'current' VAO buffer

  //Now ANY state that is related to vertex or index buffer and vertex attribute layout, is stored in the 'state' of the VAO
  glGenBuffers(1, &(drawInfo.VertexBufferID));
  glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * drawInfo.numVertices, drawInfo.vertices, GL_STATIC_DRAW);

  //Copy the index buffer into the video card to create an index buffer
  glGenBuffers(1, &(drawInfo.IndexBufferID));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * drawInfo.numIndices, drawInfo.indices, GL_STATIC_DRAW);

  //Set the vertex attributes
  int vpos_location = glGetAttribLocation(shaderProgramID, "vPos");
  if (vpos_location != -1) {
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  }
  int vnorm_location = glGetAttribLocation(shaderProgramID, "vNorm");
  if (vnorm_location != -1) {
    glEnableVertexAttribArray(vnorm_location);
    glVertexAttribPointer(vnorm_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
  }
  int vcol_location = glGetAttribLocation(shaderProgramID, "vCol");
  if (vcol_location != -1) {
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));
  }

  int vTextCoords_location = glGetAttribLocation(shaderProgramID, "vTextCoords");
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
    std::stringstream ss;
    ss << "OpenGL Error in UploadToGPU: 0x" << std::hex << err;
    this->AppendTextToLastError(ss.str(), true);
    return false;
  }

  delete[] drawInfo.vertices;
  drawInfo.vertices = nullptr;

  delete[] drawInfo.indices;
  drawInfo.indices = nullptr;

  this->modelName_to_VAOID[drawInfo.meshPath] = std::move(drawInfo);
  return true;
}

std::string VAOManager::GetLastError(bool bAndClear) {
  std::string theLastError = this->lastErrorString;
  if (bAndClear) this->lastErrorString = "";
  return theLastError;
}
void VAOManager::AppendTextToLastError(std::string text, bool addNewLineBefore) {
  std::stringstream ss;
  ss << this->lastErrorString;
  if (addNewLineBefore && !this->lastErrorString.empty()) ss << '\n';
  ss << text;
  this->lastErrorString = ss.str();
}

void VAOManager::Shutdown() {
  for (std::map<std::string, ModelDrawInfo>::iterator it = modelName_to_VAOID.begin(); it != modelName_to_VAOID.end(); ++it){
    ModelDrawInfo& drawInfo = it->second;

    if (glIsVertexArray(drawInfo.VAOID))    glDeleteVertexArrays(1, &drawInfo.VAOID);
    if (glIsBuffer(drawInfo.VertexBufferID)) glDeleteBuffers(1, &drawInfo.VertexBufferID);
    if (glIsBuffer(drawInfo.IndexBufferID))  glDeleteBuffers(1, &drawInfo.IndexBufferID);
  }

  modelName_to_VAOID.clear();
}