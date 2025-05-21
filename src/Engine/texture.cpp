#include "texture.hpp"
#include "files.hpp"
#include <cstdio>

bool Texture::createTexture(const char* path) {
	char* buffer{};
	size_t fileSize;

	if (!loadFile(buffer, path, fileSize)) {
		fprintf(stderr, "Failed to load file: %s\n", path);
		return false;
	}
	
	for (size_t i = 0; i < fileSize; ++i) {
		printf("%02X ", static_cast<unsigned char>(buffer[i]));
		if ((i + 1) % 16 == 0) printf("\n"); // new line every 16 bytes
	}
	printf("\n");


	/*unsigned int vertexCount{0}, triangleCount{0};
	const char* cursor = parseHeader(buffer, vertexCount, triangleCount);
	if (vertexCount == 0 || triangleCount == 0 || !cursor) {
		fprintf(stderr, "Invalid Number / Formatting of Vertices / Triangles\n");
		return false;
	}

	if (!allocate(vertexCount, triangleCount)) {
		return false;
	}

	numVerticesToDraw = vertexCount;
	numIndicesToDraw = triangleCount * 3;
	cursor = parseMeshVertices(*this, cursor, vertexCount, hasNormals);

	if (!cursor) {
		fprintf(stderr, "Vertex data input failed\n");
		cleanup();
		return false;
	}

	cursor = parseMeshIndices(*this, cursor, triangleCount);
	if (!cursor) {
		fprintf(stderr, "Face data input failed\n");
		cleanup();
		return false;
	}*/

	return true;
}