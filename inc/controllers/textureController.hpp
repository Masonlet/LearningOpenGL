#pragma once

#include "controller.hpp"
#include "objects/texturedata.hpp"
#include "objects/model.hpp"

struct TextureController : public Controller<TextureData> {
	bool bindTextureToModel(Model& model, TextureData& texture, unsigned int slot, float mix);

	void update(TextureData&, const InputManager&, float) override {}
};