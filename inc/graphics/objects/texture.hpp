#pragma once

#include <string>

struct Texture {
	uint32_t id{ 0 };
	int32_t  width{ 0 }, height{ 0 };
  uint8_t* pixels{ nullptr };
  uint8_t  pixelSize{ 0 };
  size_t   byteSize{ 0 };

	inline bool empty() const { return width == 0 || height == 0 || !pixels || byteSize == 0; }
  inline void freePixels() { if (pixels) { delete[] pixels; pixels = nullptr; } byteSize = 0; }

	Texture() = default;
	~Texture() { if (pixels) { delete[] pixels; pixels = nullptr; }}

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

  Texture(Texture&& other) noexcept { *this = static_cast<Texture&&>(other); }
  Texture& operator=(Texture&& other) noexcept {
    if (this != &other) {
      delete[] pixels;

      id = other.id;
      width = other.width;
      height = other.height;
      pixels = other.pixels;
      pixelSize = other.pixelSize;
      byteSize = other.byteSize;

      other.pixels = nullptr;
      other.byteSize = 0;
    }
    return *this;
  }
};