#include "models/plyParser.hpp"
#include "utils/parser.hpp"
#include "math/constants.hpp"
#include <string>

bool parsePlyMesh(const unsigned char* p, unsigned int size, MeshData& drawInfo) {
	const unsigned char* cursor = parsePlyHeader(p, drawInfo.numVertices, drawInfo.numTriangles, drawInfo.hasNormals, drawInfo.hasColours, drawInfo.hasTexCoords);
	if (!cursor) {
		fprintf(stderr, "[LoadModelFromFile ERROR] Failed to parse header or missing 'end_header'\n");
		delete[] drawInfo.indices;
		drawInfo.indices = nullptr;
		delete[] drawInfo.vertices;
		drawInfo.vertices = nullptr;
		delete[] p;
		return false;
	}

	if (drawInfo.numVertices == 0 || drawInfo.numTriangles == 0) {
		fprintf(stderr, "[LoadModelFromFile ERROR] Header found but no vertices/triangles declared\n");
		delete[] drawInfo.indices;
		drawInfo.indices = nullptr;
		delete[] drawInfo.vertices;
		drawInfo.vertices = nullptr;
		delete[] p;
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
		delete[] p;
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
		delete[] p;
		return false;
	}

	delete[] p;
	return true;
}

static const unsigned char* parsePlyElementLine(const unsigned char* p, unsigned int& verticesOut, unsigned int& trianglesOut) {
	const unsigned char* trimmed = skipWhitespace(p + 7);

	if (strncmp((const char*)trimmed, "vertex", 6) == 0 && (trimmed[6] == ' ' || trimmed[6] == '\t')) {
		trimmed = skipWhitespace(trimmed + 6);
		return parseUInt(trimmed, verticesOut);
	}
	else if (strncmp((const char*)trimmed, "face", 4) == 0 && (trimmed[4] == ' ' || trimmed[4] == '\t')) {
		trimmed = skipWhitespace(trimmed + 4);
		return parseUInt(trimmed, trianglesOut);
	}

	return p;
}

static const unsigned char* parsePlyPropertyLine(const unsigned char* p, bool& hasNx, bool& hasNy, bool& hasNz, bool& hasR, bool& hasG, bool& hasB, bool& hasU, bool& hasV) {
	const unsigned char* trimmed = skipWhitespace(p + 8);

	char temp[32]{};
	if (!(trimmed = parseToken(trimmed, (unsigned char*)temp, sizeof(temp)))) {
		fprintf(stderr, "[parsePlyHeader ERROR] Failed to parse property list count type, type: %s\n", temp);
		return nullptr;
	}

	if (strcmp(temp, "list") == 0) {
		/*
		 1 = Count Type
		 2 = Value Type
		 3 = Property Name
		*/
		char property[3][32]{};
		for (int i = 0; i < 3; ++i) {
			if (!(trimmed = parseToken(trimmed, (unsigned char*)property[i], sizeof(property[i])))) {
				fprintf(stderr, "[parsePlyHeader ERROR] Failed to parse property list number %d\n", i);
				return nullptr;
			}
		}
		return trimmed;
	}

	char propertyName[32]{};
	trimmed = parseToken(trimmed, (unsigned char*)propertyName, sizeof(propertyName));
	if (strcmp(propertyName, "nx") == 0 || strcmp(propertyName, "normal_x") == 0) hasNx = true;
	else if (strcmp(propertyName, "ny") == 0 || strcmp(propertyName, "normal_y") == 0) hasNy = true;
	else if (strcmp(propertyName, "nz") == 0 || strcmp(propertyName, "normal_z") == 0) hasNz = true;

	else if (strcmp(propertyName, "red") == 0) hasR = true;
	else if (strcmp(propertyName, "green") == 0) hasG = true;
	else if (strcmp(propertyName, "blue") == 0) hasB = true;

	else if (strcmp(propertyName, "u") == 0 || strcmp(propertyName, "texture_u") == 0) hasU = true;
	else if (strcmp(propertyName, "v") == 0 || strcmp(propertyName, "texture_v") == 0) hasV = true;

	return trimmed;
}

const unsigned char* parsePlyHeader(const unsigned char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut, bool& hasTexCoordsOut) {
	if (!p) return nullptr;

	numVerticesOut = 0;
	numTrianglesOut = 0;
	hasNormalsOut = false;
	hasColoursOut = false;
	hasTexCoordsOut = false;

	bool hasNx = false, hasNy = false, hasNz = false;
	bool hasRed = false, hasGreen = false, hasBlue = false;
	bool hasU = false, hasV = false;

	while (*p) {
		const char* lineStart = reinterpret_cast<const char*>(p);
		const char* lineEnd = reinterpret_cast<const char*>(skipToNextLine(p));
		int lineLen = static_cast<int>(lineEnd - lineStart);

		const unsigned char* trimmed = skipWhitespace(reinterpret_cast<const unsigned char*>(lineStart));

		if (strncmp((const char*)trimmed, "element", 7) == 0 && (trimmed[7] == ' ' || trimmed[7] == '\t')) {
			if (!parsePlyElementLine(trimmed, numVerticesOut, numTrianglesOut))
				return nullptr;
		}
		else if (strncmp((const char*)trimmed, "property", 8) == 0) {
			if (!parsePlyPropertyLine(trimmed, hasNx, hasNy, hasNz, hasRed, hasGreen, hasBlue, hasU, hasV))
				return nullptr;
		}
		else if (strncmp((const char*)trimmed, "end_header", 10) == 0) {
			hasNormalsOut = hasNx && hasNy && hasNz;
			hasColoursOut = hasRed && hasGreen && hasBlue;
			hasTexCoordsOut = hasU && hasV;
			return skipToNextLine(trimmed);
		}
		else if (!(strncmp((const char*)trimmed, "ply", 3) == 0)
			&& !(strncmp((const char*)trimmed, "format", 6) == 0)
			&& !(strncmp((const char*)trimmed, "comment", 7) == 0))
			fprintf(stderr, "[Warning] %.*s\n", lineLen, lineStart);

		p = reinterpret_cast<const unsigned char*>(lineEnd);
	}

	return nullptr;
}

const unsigned char* parseVertices(MeshData& drawInfo, const unsigned char* p) {
	if (!drawInfo.vertices || drawInfo.numVertices == 0) {
		fprintf(stderr, "[parseVertices ERROR] vertices buffer not allocated!\n");
		return nullptr;
	}

	if (!p) {
		fprintf(stderr, "[parseVertices ERROR] input pointer is null\n");
		return nullptr;
	}

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

		//Pos
		bool valid = true;
		PARSE_OR_INVALID(parseFloat, v.pos.x, "Failed to parse position X");
		PARSE_OR_INVALID(parseFloat, v.pos.y, "Failed to parse position Y");
		PARSE_OR_INVALID(parseFloat, v.pos.z, "Failed to parse position Z");
		v.pos.w = 1.0f;
		if (!valid) {
			p = reinterpret_cast<const unsigned char*>(lineEnd);
			continue;
		}

		//Normals
		if (drawInfo.hasNormals) {
			PARSE_OR_INVALID(parseFloat, v.norm.x, "Failed to parse normal X");
			PARSE_OR_INVALID(parseFloat, v.norm.y, "Failed to parse normal Y");
			PARSE_OR_INVALID(parseFloat, v.norm.z, "Failed to parse normal Z");
		}

		//Colour
		if (drawInfo.hasColours) {
			linePtr = skipWhitespace(linePtr);
			if (*linePtr != '\0') {
				Vec3 colour = { 1.0f, 1.0f, 1.0f };
				const unsigned char* original = linePtr;

				bool parsedFloat = true;
				PARSE_OR_INVALID(parseFloat, colour.r, "Failed to parse colour R");
				PARSE_OR_INVALID(parseFloat, colour.g, "Failed to parse colour G");
				PARSE_OR_INVALID(parseFloat, colour.b, "Failed to parse colour B");

				if (parsedFloat &&
					colour.x >= 0.0f && colour.x <= 1.0f &&
					colour.y >= 0.0f && colour.y <= 1.0f &&
					colour.z >= 0.0f && colour.z <= 1.0f) {
					v.col = Vec4{ colour.x, colour.y, colour.z, 1.0f };
					drawInfo.hasColours = true;
				}
				else {
					unsigned int ri = 0, gi = 0, bi = 0, ai = 255;
					const unsigned char* q = original;

					valid = true;
					if (!(q = parseUInt(q, ri))) valid = false;
					if (!(q = parseUInt(q, gi))) valid = false;
					if (!(q = parseUInt(q, bi))) valid = false;

					const unsigned char* parsedAlpha = parseUInt(q, ai);
					if (!parsedAlpha) ai = 255;
					else q = parsedAlpha;

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

		//TexCoords
		if (drawInfo.hasTexCoords) {
			PARSE_OR_INVALID(parseFloat, v.texCoord.x, "Failed to parse texcoord U");
			PARSE_OR_INVALID(parseFloat, v.texCoord.y, "Failed to parse texcoord V");
		}

		if (v.pos.y < minY) minY = v.pos.y;
		if (v.pos.y > maxY) maxY = v.pos.y;

		++i;
		p = reinterpret_cast<const unsigned char*>(lineEnd);
	}

	drawInfo.minY = minY;
	drawInfo.maxY = maxY;
	return p;
}

const unsigned char* parseIndices(MeshData& drawInfo, const unsigned char* p) {
	if (!drawInfo.indices || drawInfo.numIndices == 0) {
		fprintf(stderr, "[parseIndices ERROR] index buffer not allocated!\n");
		return nullptr;
	}

	if (!p) {
		fprintf(stderr, "[parseIndices ERROR] input pointer is null\n");
		return nullptr;
	}

	unsigned int triangleIndex = 0;

	while (triangleIndex < drawInfo.numTriangles && *p) {
		const unsigned char* lineStart = p;
		const unsigned char* lineEnd = skipToNextLine(p);
		size_t lineLen = lineEnd - lineStart;

		while (lineLen > 0 && (lineStart[lineLen - 1] == '\n' || lineStart[lineLen - 1] == '\r'))
			--lineLen;

		if (lineLen == 0) {
			p = lineEnd;
			continue;
		}

		const unsigned char* linePtr = skipWhitespace(lineStart);
		if (*linePtr == '\0' || *linePtr == '\n') {
			p = lineEnd;
			continue;
		}

		unsigned int count = 0;
		if (!(linePtr = parseUInt(linePtr, count))) {
			p = lineEnd;
			continue;
		}

		if (count != 3) {
			p = lineEnd;
			continue;
		}

		unsigned int i0 = 0, i1 = 0, i2 = 0;
		bool valid = true;

		if (!(linePtr = parseUInt(linePtr, i0))) valid = false;
		if (!(linePtr = parseUInt(linePtr, i1))) valid = false;
		if (!(linePtr = parseUInt(linePtr, i2))) valid = false;

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