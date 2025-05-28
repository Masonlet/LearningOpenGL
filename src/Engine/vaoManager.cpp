#include "vaoManager.hpp"
#include "parser.hpp"
#include "files.hpp"
#include "init.hpp"

#include <vector>
#include <sstream>

constexpr Vec3 DEFAULT_NORMAL{0.0f, 0.0f, 0.0f};
constexpr Vec3 DEFAULT_COLOUR{0.0f, 1.0f, 0.0f};

const static Vec3 calculateGradient(float y, float minY, float maxY) {
	float normalizedY = (y - minY) / (maxY - minY);

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
const static unsigned char* parseVertices(Vertex*& vertices, const unsigned char* p, unsigned int numVertices, const std::string& fileName, bool hasNormals = false) {
	Vec3 assignedColour{DEFAULT_COLOUR}; 
	bool randomColours{false};
	bool verticalGradient{false};

	if (fileName.find("Galactica") != std::string::npos) {
		verticalGradient = true;
	} else if (fileName.find("de--lorean") != std::string::npos) {
		assignedColour = Vec3{0.0f, 1.0f, 0.0f}; // green
	} else if (fileName.find("dolphin") != std::string::npos) {
		assignedColour = Vec3{0.0f, 0.0f, 1.0f}; // blue
	} else if (fileName.find("AUDI") != std::string::npos) {
		assignedColour = Vec3{1.0f, 1.0f, 0.0f}; // yellow
	} else if (fileName.find("homer") != std::string::npos) {
		randomColours = true;
	}
	
	//Find min and max Y values
	float minY = FLT_MAX, maxY = -FLT_MAX;
	for (unsigned int i{0}; i < numVertices; i++) {
		Vertex& v = vertices[i];

		p = parseFloat(p, v.pos.x);
		p = parseFloat(p, v.pos.y);
		p = parseFloat(p, v.pos.z);
		if (hasNormals) {
			p = parseFloat(p, v.norm.x);
			p = parseFloat(p, v.norm.y);
			p = parseFloat(p, v.norm.z);
		} else {
			v.norm = DEFAULT_NORMAL;
		}
		if (verticalGradient) {
			if (v.pos.y < minY) minY = v.pos.y;
			if (v.pos.y > maxY) maxY = v.pos.y;
		}

		p = skipToNextLine(p);
	}

	for (unsigned int i = 0; i < numVertices; ++i) {
		if (randomColours) {
			vertices[i].col = Vec3{
				static_cast<float>(rand()) / RAND_MAX,
				static_cast<float>(rand()) / RAND_MAX,
				static_cast<float>(rand()) / RAND_MAX
			};
		} else if (verticalGradient) {
			vertices[i].col = calculateGradient(vertices[i].pos.y, minY, maxY);
		} else {
			vertices[i].col = assignedColour;
		}
	}

	return p;
}
const static unsigned char* parseIndices(unsigned int*& indices, const unsigned char* p, unsigned int numTriangles) {
	for (unsigned int i = 0, b = 0; i < numTriangles; i++, b += 3) {
		unsigned int vertices{0};
		p = parseStringUInt(p, vertices);

		if (vertices != 3) {
			fprintf(stderr, "Error: Triangle %u has %u vertices (expected 3)\n", i, vertices);

			return nullptr;
		}

		p = parseStringUInt(p, indices[b + 0]);
		p = parseStringUInt(p, indices[b + 1]);
		p = parseStringUInt(p, indices[b + 2]);
		p = skipToNextLine(p);
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

	this->modelMatrix = Mat4::identity();
}

bool VAOManager::LoadModelIntoVAO(std::string fileName, ModelDrawInfo& drawInfo, unsigned int shaderProgramID) {
	//Load the model from file (Do it here since if we cant load it theres no point in doing anything else)
	drawInfo.meshName = fileName;
	if (!this->LoadTheModel(fileName, drawInfo)) {
		this->AppendTextToLastError("Didnt load model", true);
		return false;
	}
	//Model is loaded and the vertices and indices are in the drawInfo struct
	
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
	unsigned int vpos_location = 0; //vPos program
	unsigned int vcol_location = 1; //vCol program
	unsigned int vnorm_location = 2; //vNorm program;

	// Set the vertex attributes for this shader
	glEnableVertexAttribArray(vpos_location); // vPos
	glVertexAttribPointer( vpos_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( void* )0);

	glEnableVertexAttribArray(vcol_location); // vCol
	glVertexAttribPointer( vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), ( void* )( sizeof(float) * 3 ));

	glEnableVertexAttribArray(vnorm_location);
	glVertexAttribPointer(vnorm_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(float) * 6));

	// Now that all the parts are set up, set the VAO to zero
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	this->modelName_to_VAOID[drawInfo.meshName] = drawInfo;
	return true;
}

bool VAOManager::FindDrawInfoByModelName(std::string fileName, ModelDrawInfo& drawInfo) {
	std::map<std::string, ModelDrawInfo>::iterator itDrawInfo = this->modelName_to_VAOID.find(fileName);

	if (itDrawInfo == this->modelName_to_VAOID.end())
		return false;

	drawInfo = itDrawInfo->second;
	return true;
}

bool VAOManager::LoadTheModel(const std::string& path, ModelDrawInfo& drawInfo) {
	unsigned char* buffer{nullptr};
	size_t fileSize{0};

	if (!loadFile(buffer, fileSize, path)) {
		fprintf(stderr, "Failed to load file: %s\n", path.c_str());
		return false;
	}

	const unsigned char* cursor = parseHeader(buffer, drawInfo.numVertices,  drawInfo.numTriangles);
	if (drawInfo.numVertices == 0 || drawInfo.numTriangles == 0 || !cursor) {
		fprintf(stderr, "Invalid Number / Formatting of Vertices / Triangles\n");
		return false;
	}

	drawInfo.numIndices = drawInfo.numTriangles * 3;

	drawInfo.vertices = new Vertex[drawInfo.numVertices];
	cursor = parseVertices(drawInfo.vertices, cursor, drawInfo.numVertices, path);
	if (!cursor) {
		fprintf(stderr, "Vertex data input failed\n");
		return false;
	}

	drawInfo.indices = new unsigned int[drawInfo.numIndices];
	cursor = parseIndices(drawInfo.indices, cursor, drawInfo.numTriangles);
	if (!cursor) {
		fprintf(stderr, "Face data input failed\n");
		return false;
	}

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