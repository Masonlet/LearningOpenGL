#pragma once

struct TextureData;
struct TextureConnection;

bool parseTexture(const unsigned char*& p, TextureData& out);
bool parseCubeTexture(const unsigned char*& p, TextureData& out);
bool parseTextureConnection(const unsigned char*& p, TextureConnection& out);

