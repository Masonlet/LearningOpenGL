#include "models/plyParser.hpp"
#include "utils/parser.hpp"
#include "math/constants.hpp"
#include <string>

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

static const unsigned char* parsePlyPropertyLine(const unsigned char* p, bool& hasNx, bool& hasNy, bool& hasNz, bool& hasR, bool& hasG, bool& hasB) {
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
	return trimmed;
}

static bool checkPlyNormalsAndColors(bool hasNx, bool hasNy, bool hasNz, bool hasR, bool hasG, bool hasB, bool& hasNormals, bool& hasColors) {
	hasNormals = hasNx && hasNy && hasNz;
	hasColors = hasR && hasG && hasB;
	return true;
}

const unsigned char* parsePlyHeader(const unsigned char* p, unsigned int& numVerticesOut, unsigned int& numTrianglesOut, bool& hasNormalsOut, bool& hasColoursOut) {
	if (!p) return nullptr;

	numVerticesOut = 0;
	numTrianglesOut = 0;
	hasNormalsOut = false;
	hasColoursOut = false;

	bool hasNx = false, hasNy = false, hasNz = false;
	bool hasRed = false, hasGreen = false, hasBlue = false;

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
			if (!parsePlyPropertyLine(trimmed, hasNx, hasNy, hasNz, hasRed, hasGreen, hasBlue))
				return nullptr;
		}
		else if (strncmp((const char*)trimmed, "end_header", 10) == 0) {
			if (!checkPlyNormalsAndColors(hasNx, hasNy, hasNz, hasRed, hasGreen, hasBlue, hasNormalsOut, hasColoursOut))
				return nullptr;
			else
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

	const float bandSize = 1.0f / 4.0f; // 4 intervals for 5 colors
	int bandIndex = static_cast<int>(normalizedY / bandSize);
	if (bandIndex >= 4) bandIndex = 3;
	if (bandIndex < 0) bandIndex = 0;

	const float localT = (normalizedY - bandIndex * bandSize) / bandSize;

	const Vec3 color1 = colors[bandIndex];
	const Vec3 color2 = colors[bandIndex + 1];

	return {
			color1.x + (color2.x - color1.x) * localT,
			color1.y + (color2.y - color1.y) * localT,
			color1.z + (color2.z - color1.z) * localT
	};
}
const unsigned char* parseVertices(ModelDrawInfo& drawInfo, const unsigned char* p) {
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
		else v.norm = DEFAULT_NORMAL;

		//Colour
		linePtr = skipWhitespace(linePtr);
		if (*linePtr != '\0') {
			Vec3 colour = { 1.0f, 1.0f, 1.0f };
			const unsigned char* original = linePtr;
			const unsigned char* temp = linePtr;

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
				temp = original;
				unsigned int ri = 0, gi = 0, bi = 0, ai = 255;
				const unsigned char* q = temp;
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
					drawInfo.hasColours = true;
				}
			}
		}

		if (v.pos.y < minY) minY = v.pos.y;
		if (v.pos.y > maxY) maxY = v.pos.y;

		++i;
		p = reinterpret_cast<const unsigned char*>(lineEnd);
	}

	return p;
}

const unsigned char* parseIndices(ModelDrawInfo& drawInfo, const unsigned char* p) {
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