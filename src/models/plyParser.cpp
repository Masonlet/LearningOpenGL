#include "models/plyParser.hpp"
#include "utils/parser.hpp"
#include "math/constants.hpp"
#include <string>

static bool deleteMesh(const unsigned char*& p, MeshData& drawInfo, const std::string& errorMsg) {
	fprintf(stderr, "[LoadModelFromFile ERROR] Failed to parse %s\n", errorMsg.c_str());
	if(drawInfo.indices) delete[] drawInfo.indices;
	drawInfo.indices = nullptr;
	if(drawInfo.vertices) delete[] drawInfo.vertices;
	drawInfo.vertices = nullptr;
	return false;
}
bool parsePlyMesh(const unsigned char*& p, unsigned int size, MeshData& drawInfo) {
	if (!p) {
		fprintf(stderr, "[parsePlyHeader ERROR] Input pointer is null\n");
		return false;
	}
	
	if (!parsePlyHeader(p, drawInfo.numVertices, drawInfo.numTriangles, drawInfo.hasNormals, drawInfo.hasColours, drawInfo.hasTexCoords)) 
		return deleteMesh(p, drawInfo, "header or missing 'end_header'");
	
	if (drawInfo.numVertices == 0 || drawInfo.numTriangles == 0) 	
		return deleteMesh(p, drawInfo, "header, no vertices/triangles declared");
	
	drawInfo.numIndices = drawInfo.numTriangles * 3;
	drawInfo.vertices = new Vertex[drawInfo.numVertices];
	if (!parseVertices(p, drawInfo)) 
		return deleteMesh(p, drawInfo, "vertex data");

	drawInfo.indices = new unsigned int[drawInfo.numIndices];
	if (!parseIndices(p, drawInfo)) 
		return deleteMesh(p, drawInfo, "face data");

	return true;
}

static bool parsePlyElementLine(const unsigned char*& p, unsigned int& verticesOut, unsigned int& trianglesOut) {
	if (!p) {
		fprintf(stderr, "[parsePlyHeader ERROR] Input pointer is null\n");
		return false;
	}
	
	skipWhitespace(p += 7);
	if (strncmp((const char*)p, "vertex", 6) == 0 && (p[6] == ' ' || p[6] == '\t')) {
		skipWhitespace(p += 6);
		return parseUInt(p, verticesOut);
	}
	else if (strncmp((const char*)p, "face", 4) == 0 && (p[4] == ' ' || p[4] == '\t')) {
		skipWhitespace(p += 4);
		return parseUInt(p, trianglesOut);
	}
	return false;
}

static bool parsePlyPropertyLine(const unsigned char*& p, bool& hasNx, bool& hasNy, bool& hasNz, bool& hasR, bool& hasG, bool& hasB, bool& hasU, bool& hasV) {
	if (!p) {
		fprintf(stderr, "[parsePlyHeader ERROR] Input pointer is null\n");
		return false;
	}

	skipWhitespace(p += 8);

	char type[32]{};
	if (!parseToken(p, (unsigned char*)type, sizeof(type))) {
		fprintf(stderr, "[parsePlyHeader ERROR] Failed to parse property list count type, type: %s\n", type);
		return false;
	}

	if (strcmp(type, "list") == 0) {
		/*
		 1 = Count Type
		 2 = Value Type
		 3 = Property Name
		*/
		char property[3][32]{};
		for (int i = 0; i < 3; ++i) {
			if (!parseToken(p, reinterpret_cast<unsigned char*>(property[i]), sizeof(property[i]))) {
				fprintf(stderr, "[parsePlyHeader ERROR] Failed to parse property list number %d\n", i);
				return false;
			}
		}

		return true;
	}

	char propertyName[32]{};
	if(!parseToken(p, (unsigned char*)propertyName, sizeof(propertyName))) {
		fprintf(stderr, "[parsePlyHeader ERROR] Failed to parse property name\n");
		return false;
	}

	if (strcmp(propertyName, "nx") == 0 || strcmp(propertyName, "normal_x") == 0) hasNx = true;
	else if (strcmp(propertyName, "ny") == 0 || strcmp(propertyName, "normal_y") == 0) hasNy = true;
	else if (strcmp(propertyName, "nz") == 0 || strcmp(propertyName, "normal_z") == 0) hasNz = true;
	else if (strcmp(propertyName, "red") == 0) hasR = true;
	else if (strcmp(propertyName, "green") == 0) hasG = true;
	else if (strcmp(propertyName, "blue") == 0) hasB = true;
	else if (strcmp(propertyName, "u") == 0 || strcmp(propertyName, "texture_u") == 0) hasU = true;
	else if (strcmp(propertyName, "v") == 0 || strcmp(propertyName, "texture_v") == 0) hasV = true;
	return true;
}

bool parsePlyHeader(const unsigned char*& p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut, bool& hasTexCoordsOut) {
	if (!p) {
		fprintf(stderr, "[parseVertices ERROR] input pointer is null\n");
		return false;
	}
	skipWhitespace(p);
	bool hasNx = false, hasNy = false, hasNz = false;
	bool hasRed = false, hasGreen = false, hasBlue = false;
	bool hasU = false, hasV = false;

	while (*p) {
		const unsigned char* lineEnd = p;
		skipToNextLine(lineEnd);
		trimEOL(p, lineEnd);

		if (lineEnd == p) {
			p++;
			continue;
		}

		if (strncmp((const char*)p, "element", 7) == 0 && (p[7] == ' ' || p[7] == '\t')) {
			if(!parsePlyElementLine(p, numVerticesOut, numTrianglesOut)) 
				return false;
		}
		else if (strncmp((const char*)p, "property", 8) == 0) {
			if (!parsePlyPropertyLine(p, hasNx, hasNy, hasNz, hasRed, hasGreen, hasBlue, hasU, hasV))	
				return false;
		}
		else if (strncmp((const char*)p, "end_header", 10) == 0) {
			hasNormalsOut = hasNx && hasNy && hasNz;
			hasColoursOut = hasRed && hasGreen && hasBlue;
			hasTexCoordsOut = hasU && hasV;
			p = lineEnd;
			return true;
		}
		else if (!(strncmp((const char*)p, "ply", 3)     == 0)
		    	&& !(strncmp((const char*)p, "format", 6)  == 0)
			    && !(strncmp((const char*)p, "comment", 7) == 0))
			fprintf(stderr, "[Warning] %.*s\n", static_cast<int>(lineEnd - p), (const char*)p);

		p = lineEnd;
	}

	return false;
}

bool parseVertices(const unsigned char*& p, MeshData& drawInfo) {
	if (!p) {
		fprintf(stderr, "[parseVertices ERROR] input pointer is null\n");
		return false;
	}

	if (!drawInfo.vertices || drawInfo.numVertices == 0) {
		fprintf(stderr, "[parseVertices ERROR] vertices buffer not allocated!\n");
		return false;
	}

	float minY = FLT_MAX, maxY = -FLT_MAX;
	unsigned int i = 0;
	while (i < drawInfo.numVertices && *p) {
		Vertex& v = drawInfo.vertices[i];
		const unsigned char* lineEnd = p; 
		skipToNextLine(lineEnd);
		trimEOL(p, lineEnd);

		if (lineEnd == p) {
			p++;
			continue;
		}

		if(*p == '\0')  return false;
		
		bool valid = true;
		PARSE_OR_INVALID(parseFloat, v.pos.x, "Failed to parse position X");
		PARSE_OR_INVALID(parseFloat, v.pos.y, "Failed to parse position Y");
		PARSE_OR_INVALID(parseFloat, v.pos.z, "Failed to parse position Z");
		v.pos.w = 1.0f;
		if (!valid) {
			p = lineEnd;
			continue;
		}

		if (drawInfo.hasNormals) {
			PARSE_OR_INVALID(parseFloat, v.norm.x, "Failed to parse normal X");
			PARSE_OR_INVALID(parseFloat, v.norm.y, "Failed to parse normal Y");
			PARSE_OR_INVALID(parseFloat, v.norm.z, "Failed to parse normal Z");
		}

		if (drawInfo.hasColours) {
			if (*p != '\0') {
				Vec3 colour = { 1.0f, 1.0f, 1.0f };
				const unsigned char* original = p;
				PARSE_OR_INVALID(parseFloat, colour.r, "Failed to parse float colour R");
				PARSE_OR_INVALID(parseFloat, colour.g, "Failed to parse float colour G");
				PARSE_OR_INVALID(parseFloat, colour.b, "Failed to parse float colour B");

				if (valid &&
					colour.x >= 0.0f && colour.x <= 1.0f &&
					colour.y >= 0.0f && colour.y <= 1.0f &&
					colour.z >= 0.0f && colour.z <= 1.0f) {
					v.col = Vec4{ colour.x, colour.y, colour.z, 1.0f };
					drawInfo.hasColours = true;
				}
				else {
					p = original;
					unsigned int ri = 0, gi = 0, bi = 0, ai = 256;

					valid = true;
					PARSE_OR_INVALID(parseUInt, ri, "");
					PARSE_OR_INVALID(parseUInt, gi, "");
					PARSE_OR_INVALID(parseUInt, bi, "");
					if (!parseUInt(p, ai)) ai = 255;

					if (valid && ri <= 255 && gi <= 255 && bi <= 255) {
						v.col = Vec4{
								static_cast<float>(ri) / 255.0f,
								static_cast<float>(gi) / 255.0f,
								static_cast<float>(bi) / 255.0f,
								static_cast<float>(ai) / 255.0f
						};
					}
				}
			}
		}

		if (drawInfo.hasTexCoords) {
			PARSE_OR_INVALID(parseFloat, v.texCoord.x, "Failed to parse texcoord U");
			PARSE_OR_INVALID(parseFloat, v.texCoord.y, "Failed to parse texcoord V");
		}

		if (v.pos.y < minY) minY = v.pos.y;
		if (v.pos.y > maxY) maxY = v.pos.y;

		++i;
		p = lineEnd;
	}

	drawInfo.minY = minY;
	drawInfo.maxY = maxY;
	return true;
}

bool parseIndices(const unsigned char*& p, MeshData& drawInfo) {
	if (!p) {
		fprintf(stderr, "[parseIndices ERROR] input pointer is null\n");
		return false;
	}
	
	if (!drawInfo.indices || drawInfo.numIndices == 0) {
		fprintf(stderr, "[parseIndices ERROR] index buffer not allocated!\n");
		return false;
	}

	unsigned int triangleIndex = 0;
	while (triangleIndex < drawInfo.numTriangles && *p) {
		const unsigned char* lineEnd = p;
		skipToNextLine(lineEnd);
		trimEOL(p, lineEnd);

		if (lineEnd == p) {
			p++;
			continue;
		}

		unsigned int count = 0;
		if (!parseUInt(p, count)) {
			p = lineEnd;
			continue;
		}

		if (count != 3) {
			p = lineEnd;
			continue;
		}

		unsigned int i0{ 0 }, i1{ 0 }, i2{ 0 };
		bool valid = true;
		PARSE_OR_INVALID(parseUInt, i0, "Failed to parse indice 1");
		PARSE_OR_INVALID(parseUInt, i1, "Failed to parse indice 2");
		PARSE_OR_INVALID(parseUInt, i2, "Failed to parse indice 3");

		if (valid) {
			unsigned int base = triangleIndex * 3;
			drawInfo.indices[base + 0] = i0;
			drawInfo.indices[base + 1] = i1;
			drawInfo.indices[base + 2] = i2;
			++triangleIndex;
		}

		p = lineEnd;
	}

	return p;
}