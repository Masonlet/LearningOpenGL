#pragma once

#include "math/vertex.hpp"
#include "math/mat4.hpp"
#include "core/colour.hpp"

#include <string>
#include <map>

/*
ModelDrawInfo
* Represents a single mesh (3d object) structure ' INDEXED' format
* Its layout is set up to match how the GPU sees the mesh, not how the file was, etc
* Its also storing the information we need to tell the GPU which model we want to draw.
*/
struct ModelDrawInfo {
	ModelDrawInfo();

	std::string meshPath;

	unsigned int VAO_ID;

	unsigned int VertexBufferID, IndexBufferID;
	unsigned int VertexBuffer_Start_Index, IndexBuffer_Start_Index;
	unsigned int numVertices, numIndices, numTriangles;

	Vertex* vertices; 
	unsigned int* indices;
  Vec4 colour;

  bool hasNormals, hasColours;
  
  ColourMode colourMode;
	Mat4 modelMatrix;
};

class VAOManager {
public:
	bool LoadPrimitiveIntoVAO(ModelDrawInfo& drawInfo, unsigned int shaderProgramID);
	bool LoadModelFromFile(const std::string& path, ModelDrawInfo& drawInfo);
	bool LoadModelIntoVAO(std::string fileName, ModelDrawInfo& drawInfo, unsigned int shaderProgramID);

	bool FindDrawInfoByModelName(std::string fileName, ModelDrawInfo& drawInfo);

	std::string GetLastError(bool bAndClear = true);

private:
	std::map<std::string, ModelDrawInfo> modelName_to_VAOID;

	bool UploadToGPU(ModelDrawInfo& info, unsigned int shaderProgramID);

	std::string lastErrorString;
	void AppendTextToLastError(std::string text, bool addNewLineBefore = true);
};
