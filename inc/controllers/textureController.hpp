#pragma once

#include "controller.hpp"
#include "objects/texturedata.hpp"
#include "objects/model.hpp"

class TextureController : public Controller<TextureData> {
public:
	unsigned int currentTexture{ 0 };
	inline void incrementTexture(std::size_t textureCount) { if (textureCount != 0 && static_cast<size_t>(currentTexture + 1) < textureCount) currentTexture++; }
	inline void decrementTexture() { if (currentTexture != 0) currentTexture--; }

	void update(TextureData& light, const InputManager& input, float deltaTime) override {};
	bool bindTextureToModel(Model& model, TextureData& texture, unsigned int slot, float mix);
};