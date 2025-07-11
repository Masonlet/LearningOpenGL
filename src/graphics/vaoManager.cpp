#include "graphics/vaoManager.hpp"
#include "utils/parser.hpp"
#include "utils/files.hpp"
#include "core/engine.hpp"

#include <vector>
#include <sstream>
#include <cfloat>

#define PARSE_OR_CONTINUE(parser, target, errorMsg) \
    if (!(linePtr = parser(linePtr, target))) { \
        fprintf(stderr, "[vaoManager ERROR]: %s\n", errorMsg); \
        valid = false; \
    } else { \
      if (*linePtr == ',') ++linePtr; \
    }
    
constexpr Vec4 DEFAULT_NORMAL{ 0.0f, 0.0f, 0.0f, 0.0f };
constexpr Vec4 DEFAULT_COLOUR{ 0.0f, 1.0f, 0.0f, 0.0f };

const static Vec3 calculateGradient(float y, float minY, float maxY) {
    float range = maxY - minY;
    float normalizedY = (range == 0.0f) ? 0.0f : (y - minY) / range;

    Vec3 colors[5] = {
        Vec3{1.0f, 0.0f, 0.0f},  // Red 
        Vec3{1.0f, 0.5f, 0.0f},  // Orange
        Vec3{1.0f, 1.0f, 0.0f},  // Yellow
        Vec3{0.0f, 1.0f, 0.0f},  // Green
        Vec3{0.0f, 0.0f, 1.0f}   // Blue 
    };

    float bandSize = 1.0f / 4.0f; // 4 intervals for 5 colors
    int bandIndex = static_cast<int>(normalizedY / bandSize);

    if (bandIndex >= 4) bandIndex = 3;
    if (bandIndex < 0) bandIndex = 0;

    float localT = (normalizedY - bandIndex * bandSize) / bandSize;

    Vec3 color1 = colors[bandIndex];
    Vec3 color2 = colors[bandIndex + 1];

    return Vec3{
        color1.x + (color2.x - color1.x) * localT,
        color1.y + (color2.y - color1.y) * localT,
        color1.z + (color2.z - color1.z) * localT
    };
}
const static unsigned char* parseVertices(ModelDrawInfo& drawInfo, const unsigned char* p) {
    if (!drawInfo.vertices || drawInfo.numVertices == 0) {
        fprintf(stderr, "[parseVertices ERROR] vertices buffer not allocated!\n");
        return nullptr;
    }

    if (!p) {
        fprintf(stderr, "[parseVertices ERROR] input pointer is null\n");
        return nullptr;
    }

    if (drawInfo.colourMode == ColourMode::Random ||
        drawInfo.colourMode == ColourMode::VerticalGradient ||
        drawInfo.colourMode == ColourMode::Solid) 
        drawInfo.hasColours = false;

    float minY = FLT_MAX, maxY = -FLT_MAX;
    unsigned int i = 0;
    while (i < drawInfo.numVertices && *p) {
      Vertex& v = drawInfo.vertices[i];

      const char* lineStart = reinterpret_cast<const char*>(p);
      const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
      size_t lineLen = lineEnd - lineStart;

      while (lineLen > 0 && (lineStart[lineLen - 1] == '\n' || lineStart[lineLen - 1] == '\r'))
        --lineLen;

      if (lineLen == 0) {
        p = reinterpret_cast<const unsigned char*>(lineEnd);
        continue;
      }

      const unsigned char* linePtr = reinterpret_cast<const unsigned char*>(lineStart);
        linePtr = skipWhitespace(linePtr);
        if (*linePtr == '\0' || *linePtr == '\n') {
            linePtr = reinterpret_cast<const unsigned char*>(lineEnd);
            continue;
        }

        std::string badLine(lineStart, lineStart + lineLen);

        //Pos
        bool valid = true; 
        PARSE_OR_CONTINUE(parseFloat, v.pos.x, "Failed to parse position X");
        if (!valid) {
          fprintf(stderr, "Line contents: \"%s\"\n", badLine.c_str());
        }

        PARSE_OR_CONTINUE(parseFloat, v.pos.y, "Failed to parse position Y");
        PARSE_OR_CONTINUE(parseFloat, v.pos.z, "Failed to parse position Z");
        v.pos.w = 1.0f;
  
        if (!valid) {
            p = reinterpret_cast<const unsigned char*>(lineEnd);
            continue;
        }

        //Normals
        if (drawInfo.hasNormals) {
            PARSE_OR_CONTINUE(parseFloat, v.norm.x, "Failed to parse normal X");
            PARSE_OR_CONTINUE(parseFloat, v.norm.y, "Failed to parse normal Y");
            PARSE_OR_CONTINUE(parseFloat, v.norm.z, "Failed to parse normal Z");
            v.norm.w = 0.0f;
        } 
        else v.norm = DEFAULT_NORMAL;

        //Colour
        if (drawInfo.colourMode == ColourMode::PLYColour) {
            linePtr = skipWhitespace(linePtr);
            if (*linePtr != '\0') {
                Vec3 colour = { 1.0f, 1.0f, 1.0f };
                const unsigned char* original = linePtr;
                const unsigned char* temp = linePtr;

                bool parsedFloat = true;
                PARSE_OR_CONTINUE(parseFloat, colour.r, "Failed to parse colour R");
                PARSE_OR_CONTINUE(parseFloat, colour.g, "Failed to parse colour G");
                PARSE_OR_CONTINUE(parseFloat, colour.b, "Failed to parse colour B");

                if (parsedFloat && 
                    colour.x >= 0.0f && colour.x <= 1.0f && 
                    colour.y >= 0.0f && colour.y <= 1.0f && 
                    colour.z >= 0.0f && colour.z <= 1.0f) {
                    v.col = Vec4{ colour.x, colour.y, colour.z, 1.0f };
                    drawInfo.hasColours = true;
                } else {
                    temp = original;
                    unsigned int ri = 0, gi = 0, bi = 0, ai = 255;
                    const unsigned char* q = temp;
                    valid = true;

                    if (!(q = parseStringUInt(q, ri))) {
                        //fprintf(stderr, "[VERTEX PARSE WARNING] failed to parse colour int R, Line: %s\n", badLine.c_str());
                        valid = false;
                    }
                    if (!(q = parseStringUInt(q, gi))) {
                        //fprintf(stderr, "[VERTEX PARSE WARNING] failed to parse colour int G, Line: %s\n", badLine.c_str());
                        valid = false;
                    }
                    if (!(q = parseStringUInt(q, bi))) {
                        //fprintf(stderr, "[VERTEX PARSE WARNING] failed to parse colour int B, Line: %s\n", badLine.c_str());
                        valid = false;
                    }

                    const unsigned char* parsedAlpha = parseStringUInt(q, ai);
                    if (!parsedAlpha) ai = 255;
                    else q = parsedAlpha;

                    if (valid && ri <= 255 && gi <= 255 && bi <= 255) {
                        v.col = Vec4{
                            static_cast<float>(ri) / 255.0f,
                            static_cast<float>(gi) / 255.0f,
                            static_cast<float>(bi) / 255.0f,
                            static_cast<float>(ai) / 255.0f
                        };            
                        drawInfo.hasColours = true;
                    }
                }
            }
        }
        else if (drawInfo.colourMode == ColourMode::VerticalGradient) {
            if (v.pos.y < minY) minY = v.pos.y;
            if (v.pos.y > maxY) maxY = v.pos.y;
        }

        ++i;
        p = reinterpret_cast<const unsigned char*>(lineEnd);
    }

    // Colour Fill
    for (unsigned int i = 0; i < drawInfo.numVertices; ++i) {
        if (!drawInfo.hasColours) {
            switch (drawInfo.colourMode) {
            case ColourMode::Random:
                drawInfo.vertices[i].col = Vec4{
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                1.0f
                };
                break;
            case ColourMode::VerticalGradient:
                drawInfo.vertices[i].col = Vec4{calculateGradient(drawInfo.vertices[i].pos.y, minY, maxY), 1.0f};
                break;
            case ColourMode::Solid:
                drawInfo.vertices[i].col = Vec4{drawInfo.colour.x, drawInfo.colour.y, drawInfo.colour.z, 1.0f};
                break;
            default:
                drawInfo.vertices[i].col =  Vec4{DEFAULT_COLOUR.x, DEFAULT_COLOUR.y, DEFAULT_COLOUR.z, 1.0f};
                break;
            }
        }
    }

    return p;
}
const static unsigned char* parseIndices(ModelDrawInfo& drawInfo,  const unsigned char* p) {
  if (!drawInfo.indices || drawInfo.numIndices == 0) {
    fprintf(stderr, "[parseIndices ERROR] index buffer not allocated!\n");
    return nullptr;
  }

  if (!p) {
    fprintf(stderr, "[parseIndices ERROR] input pointer is null\n");
    return nullptr;
  }

  unsigned int triangleIndex = 0;

  while (triangleIndex < drawInfo.numTriangles && *p){
    const char* lineStart = reinterpret_cast<const char*>(p);
    const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
    size_t lineLen = lineEnd - lineStart;

    while (lineLen > 0 && (lineStart[lineLen - 1] == '\n' || lineStart[lineLen - 1] == '\r'))
      --lineLen; 

    if (lineLen == 0) {
      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }

    const unsigned char* linePtr = reinterpret_cast<const unsigned char*>(lineStart);
    linePtr = skipWhitespace(linePtr);

    if (*linePtr == '\0' || *linePtr == '\n') {
      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }

    std::string badLine(lineStart, lineStart + lineLen);

    unsigned int count = 0;
    if (!(linePtr = parseStringUInt(linePtr, count))) {
      fprintf(stderr, "[parseIndices ERROR] Failed to read face count, Line: %s\n", badLine.c_str());
      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }

    if (count != 3) {
      fprintf(stderr, "[parseIndices WARNING] Skipping non-triangle face (%u), Line: %s\n", count, badLine.c_str());
      p = reinterpret_cast<const unsigned char*>(lineEnd);
      continue;
    }

    unsigned int i0 = 0, i1 = 0, i2 = 0;
    bool valid = true;

    if (!(linePtr = parseStringUInt(linePtr, i0))) {
      fprintf(stderr, "[parseIndices ERROR] Failed to parse index 0, Line: %s\n", badLine.c_str());
      valid = false;
    }
    if (!(linePtr = parseStringUInt(linePtr, i1))) {
      fprintf(stderr, "[parseIndices ERROR] Failed to parse index 1, Line: %s\n", badLine.c_str());
      valid = false;
    }
    if (!(linePtr = parseStringUInt(linePtr, i2))) {
      fprintf(stderr, "[parseIndices ERROR] Failed to parse index 2, Line: %s\n", badLine.c_str());
      valid = false;
    }

    if (valid) {
      unsigned int base = triangleIndex * 3;
      drawInfo.indices[base + 0] = i0;
      drawInfo.indices[base + 1] = i1;
      drawInfo.indices[base + 2] = i2;
      ++triangleIndex;
    }

    p = reinterpret_cast<const unsigned char*>(lineEnd);
  }

  return p;
}

ModelDrawInfo::ModelDrawInfo() {
  this->VAO_ID = 0;

  this->VertexBufferID = 0;
  this->VertexBuffer_Start_Index = 0;
  this->numVertices = 0;

  this->IndexBufferID = 0;
  this->IndexBuffer_Start_Index = 0;
  this->numIndices = 0;
  this->numTriangles = 0;

  //"Local" i.e. "CPU Side" temporary array
  this->vertices = nullptr;
  this->indices = nullptr;

  this->colour = DEFAULT_COLOUR;

  this->hasNormals = false;
  this->hasColours = false;

  this->colourMode = ColourMode::PLYColour;
  this->modelMatrix = Mat4::identity();
}

bool VAOManager::LoadPrimitiveIntoVAO(ModelDrawInfo& drawInfo, unsigned int shaderProgramID){
  if (!UploadToGPU(drawInfo, shaderProgramID)) {
    AppendTextToLastError("Failed to upload primitive to GPU", true);
    return false;
  }

  return true;
}
bool VAOManager::LoadModelIntoVAO(std::string fileName, ModelDrawInfo& drawInfo, unsigned int shaderProgramID) {
  //Load the model from file (Do it here since if we cant load it theres no point in doing anything else)
  drawInfo.meshPath = fileName;
  drawInfo.hasNormals = false;
  drawInfo.hasColours = false;
  drawInfo.numVertices = 0;
  drawInfo.numTriangles = 0;

  if (!this->LoadModelFromFile(fileName, drawInfo)) {
    this->AppendTextToLastError("Didnt load model", true);
    return false;
  }
  //Model is loaded and the vertices and indices are in the drawInfo struct

  if (!drawInfo.vertices || !drawInfo.indices || drawInfo.numVertices == 0 || drawInfo.numIndices == 0) {
    this->AppendTextToLastError("Invalid model data — skipping GPU upload", true);
    return false;
  }

  return UploadToGPU(drawInfo, shaderProgramID);
}

bool VAOManager::FindDrawInfoByModelName(std::string fileName, ModelDrawInfo& drawInfo) {
  std::map<std::string, ModelDrawInfo>::iterator itDrawInfo = this->modelName_to_VAOID.find(fileName);

  if (itDrawInfo == this->modelName_to_VAOID.end())
    return false;

  drawInfo = itDrawInfo->second;
  return true;
}

bool VAOManager::LoadModelFromFile(const std::string& path, ModelDrawInfo& drawInfo) {
  unsigned char* src{ nullptr };
  if (!loadBinaryFile(src, "assets/models/" + path)) {
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
  //Ask OpenGL for a new buffer ID
  glGenVertexArrays(1, &(drawInfo.VAO_ID));

  //Bind the buffer: aka "make this the 'current' VAO buffer
  glBindVertexArray(drawInfo.VAO_ID);

  //Now ANY state that is related to vertex or index buffer and vertex attribute layout, is stored in the 'state' of the VAO
  glGenBuffers(1, &(drawInfo.VertexBufferID));
  glBindBuffer(GL_ARRAY_BUFFER, drawInfo.VertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * drawInfo.numVertices, drawInfo.vertices, GL_STATIC_DRAW);

  //Copy the index buffer into the video card to create an index buffer
  glGenBuffers(1, &(drawInfo.IndexBufferID));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, drawInfo.IndexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * drawInfo.numIndices, drawInfo.indices, GL_STATIC_DRAW);

  //Set the vertex attributes
  GLint vpos_location = glGetAttribLocation(shaderProgramID, "vPos");
  if (vpos_location != -1) {
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  }

  GLint vnorm_location = glGetAttribLocation(shaderProgramID, "vNorm");
  if (vnorm_location != -1) {
    glEnableVertexAttribArray(vnorm_location);
    glVertexAttribPointer(vnorm_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, norm));
  }

  GLint vcol_location = glGetAttribLocation(shaderProgramID, "vCol");
  if (vcol_location != -1) {
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, col));
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

  this->modelName_to_VAOID[drawInfo.meshPath] = drawInfo;
  return true;
}

std::string VAOManager::GetLastError(bool bAndClear) {
  std::string theLastError = this->lastErrorString;
  if (bAndClear) {
    this->lastErrorString = "";
  }

  return theLastError;
}
void VAOManager::AppendTextToLastError(std::string text, bool addNewLineBefore) {
  std::stringstream ss;
  ss << this->lastErrorString;
  if (addNewLineBefore && !this->lastErrorString.empty())
    ss << '\n';
  ss << text;
  this->lastErrorString = ss.str();
}
