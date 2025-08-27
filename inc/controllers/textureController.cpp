#include "controllers/textureController.hpp"
#include "utils/log.hpp"

bool TextureController::bindTextureToModel(Model& model, TextureData& texture, unsigned int slot, float mix) {
	if (slot >= Model::NUM_TEXTURES) return error("Scene", "bindTextureToModel", "slot out of range: " + std::to_string(slot));

	if (texture.name.empty() || mix <= 0.0f) {
		bool any = false;
		for (unsigned i = 0; i < Model::NUM_TEXTURES; ++i)
			if (!model.textureNames[i].empty()) {
				any = true;
				break;
			}
		model.useTextures = any;
		return debugLog("Scene", "bindTextureToModel", "unbind: " + model.name + "[slot " + std::to_string(slot) + "]", true);
	}

	model.useTextures = true;
	model.textureNames[slot] = texture.name;
	model.textureMixRatio[slot] = (mix < 0.0f) ? 0.0f : (mix > 1.0f) ? 1.0f : mix;
	return debugLog("Scene", "bindTextureToModel", "bind: " + texture.name + " to " + model.name + " [slot " + std::to_string(slot) + "], mix=" + std::to_string(mix), true);
}