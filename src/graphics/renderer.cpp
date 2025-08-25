#include <glad/glad.h>
#include "graphics/renderer.hpp"
#include "utils/log.hpp"

bool Renderer::setProgram(unsigned int program) {
	if (program == 0) return error("Renderer", "setProgram", "Program is 0");
	this->program = program;
	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "textSampler2D_00"), 0);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_01"), 1);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_02"), 2);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_03"), 3);

	modelLightedLocation = glGetUniformLocation(program, "bLighted");
	if (modelLightedLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: bLighted");

	modelLocation = glGetUniformLocation(program, "mModel");
	if (modelLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: mModel");

	modelViewLocation = glGetUniformLocation(program, "mView");
	if (modelViewLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: mView");

	modelProjectionLocation = glGetUniformLocation(program, "mProj");
	if (modelProjectionLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: mProj");

	modelInverseTransposeLocation = glGetUniformLocation(program, "mModel_InverseTranspose");
	if (modelInverseTransposeLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: mModel_InverseTranspose");

	modelSpecularLocation = glGetUniformLocation(program, "vertSpecular");
	if (modelSpecularLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: vertSpecular");

	modelUseTexturesLocation = glGetUniformLocation(program, "bUseTextures");
	if (modelUseTexturesLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: bUseTextures");

	modelColourModeLocation = glGetUniformLocation(program, "colourMode");
	if (modelColourModeLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: colourMode");

	modelColourOverrideLocation = glGetUniformLocation(program, "colourOverride");
	if (modelColourOverrideLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: colourOverride");

	modelHasVertColourLocation = glGetUniformLocation(program, "hasVertexColour");
	if (modelHasVertColourLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: hasVertexColour");

	modelMinYMaxYLocation = glGetUniformLocation(program, "yMin_yMax");
	if (modelMinYMaxYLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: yMin_yMax");

	modelSeedLocation = glGetUniformLocation(program, "seed");
	if (modelSeedLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: seed");

	modelIsSkyboxLocation = glGetUniformLocation(program, "bIsSkybox");
	if (modelIsSkyboxLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: bIsSkybox");

	skyboxTextureLocation = glGetUniformLocation(program, "skyboxCubeTexture");
	if (skyboxTextureLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: skyboxCubeTexture");
	glUniform1i(skyboxTextureLocation, SKYBOX_TU);

	eyeLocation = glGetUniformLocation(program, "eyePos");
	if (eyeLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: eyePos");

	lightCountLocation = glGetUniformLocation(program, "lightCount");
	if (lightCountLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: lightCount");

	modelTexMixRatiosLocation = glGetUniformLocation(program, "texMixRatios");
	if (modelTexMixRatiosLocation < 0) return error("Renderer", "setProgram", "Could not find uniform: texMixRatios");

	return debugLog("setProgram", "Successfully set shader program and located uniforms", true);
}

void Renderer::updateCameraUniforms(const Vec3& eye, const Mat4& view, const Mat4& projection) const {
	glUniform3f(eyeLocation, eye.x, eye.y, eye.z);
	glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, view.ptr());
	glUniformMatrix4fv(modelProjectionLocation, 1, GL_FALSE, projection.ptr());
}

static Vec3 seedFromName(const std::string& s) {
	float r = 0.0f, g = 0.0f, b = 0.0f;
	int i = 0;
	for (unsigned char c : s) {
		if (i % 3 == 0) r += static_cast<float>(c);
		else if (i % 3 == 1) g += static_cast<float>(c);
		else                 b += static_cast<float>(c);
		++i;
	}
	r = fmod(r / 255.0f, 1.0f);
	g = fmod(g / 255.0f, 1.0f);
	b = fmod(b / 255.0f, 1.0f);
	return { r, g, b };
}
bool Renderer::drawModel(MeshManager& meshManager, SceneManager& sceneManager, const ModelData& instance) const {
	if (!instance.isVisible) return true;

	MeshData* data;
	if (!meshManager.getMesh(instance.meshPath, data)) return error("Renderer", "drawModel", "Could not find mesh: " + instance.meshPath);
	 
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, instance.modelMatrix.models);
	glUniformMatrix4fv(modelInverseTransposeLocation, 1, GL_FALSE, instance.modelMatrix.inverse().transpose().models);
	glUniform4fv(modelSpecularLocation, 1, &instance.specular.x);

	glUniform1i(modelColourModeLocation, static_cast<int>(instance.colourMode));
	glUniform4fv(modelColourOverrideLocation, 1, &instance.colour.x);
	glUniform1i(modelHasVertColourLocation, data->hasColours ? 1 : 0);
	glUniform2f(modelMinYMaxYLocation, data->minY, data->maxY);

	const Vec3 randSeed = seedFromName(instance.name);
	glUniform3f(modelSeedLocation, randSeed.x, randSeed.y, randSeed.z);

	glUniform1i(modelUseTexturesLocation, instance.useTextures ? 1 : 0);
	if (instance.useTextures) {
		glUniform4f(modelTexMixRatiosLocation, instance.textureMixRatio[0], instance.textureMixRatio[1], instance.textureMixRatio[2], instance.textureMixRatio[3]);

		for (size_t i = 0; i < instance.NUM_TEXTURES; ++i) {
			const std::string& name = instance.textureNames[i];
			if (name.empty()) continue;

			int textureID = sceneManager.scene.getTextureIDFromName(name);
			if (textureID == 0) return error("Renderer", "drawModel", "Could not find texture: " + name);

			glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(i));
			glBindTexture(GL_TEXTURE_2D, textureID);
		}
	}

	glUniform1i(modelLightedLocation, instance.isLighted ? 1 : 0);

	if (instance.colour.w < 1.0f)	glDepthMask(GL_FALSE);
	glBindVertexArray(data->VAOID);
	glDrawElements(GL_TRIANGLES, data->numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (instance.colour.w < 1.0f) glDepthMask(GL_TRUE);

	return true;
}

void Renderer::bindSkyboxTexture(unsigned int textureID) const {
	glActiveTexture(GL_TEXTURE0 + SKYBOX_TU);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
}

void Renderer::updateLightCount(int count) const { 
	glUniform1i(lightCountLocation, count); 
}

void Renderer::setModelIsSkybox(bool isSkybox) const { 
	glUniform1i(modelIsSkyboxLocation, isSkybox ? 1 : 0); 
}
