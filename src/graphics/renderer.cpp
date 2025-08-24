#include <glad/glad.h>
#include "graphics/renderer.hpp"
#include "utils/log.hpp"

void Renderer::setProgram(unsigned int program) {
	this->program = program;
	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "textSampler2D_00"), 0);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_01"), 1);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_02"), 2);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_03"), 3);

	modelLightedLocation = glGetUniformLocation(program, "bLighted");
	modelLocation = glGetUniformLocation(program, "mModel");
	modelViewLocation = glGetUniformLocation(program, "mView");
	modelProjectionLocation = glGetUniformLocation(program, "mProj");
	modelInverseTransposeLocation = glGetUniformLocation(program, "mModel_InverseTranpose");
	modelSpecularLocation = glGetUniformLocation(program, "vertSpecular");
	modelUseTexturesLocation = glGetUniformLocation(program, "bUseTextures");
	modelColourModeLocation = glGetUniformLocation(program, "colourMode");
	modelColourOverrideLocation = glGetUniformLocation(program, "colourOverride");
	modelHasVertColourLocation = glGetUniformLocation(program, "hasVertexColour");
	modelMinYMaxYLocation = glGetUniformLocation(program, "yMin_yMax");
	modelSeedLocation = glGetUniformLocation(program, "seed");
	modelIsSkyboxLocation = glGetUniformLocation(program, "bIsSkybox");
	modelSkyboxTextureLocation = glGetUniformLocation(program, "skyboxCubeTexture");
	eyeLocation = glGetUniformLocation(program, "eyePos");
	lightCountLocation = glGetUniformLocation(program, "lightCount");
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
bool Renderer::drawModel(MeshManager& meshManager, SceneManager& sceneManager, const ModelData& instance, const Mat4& view, const Mat4& projection) const {
	if (!instance.isVisible) return true;

	MeshData* data;
	if (!meshManager.getMesh(instance.meshPath, data)) return error("Renderer", "drawModel", "Could not find mesh: " + instance.meshPath);
	 
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, instance.modelMatrix.models);
	Mat4 modelIT = instance.modelMatrix.inverse().transpose();
	glUniformMatrix4fv(modelInverseTransposeLocation, 1, GL_FALSE, modelIT.models);
	glUniform4fv(modelSpecularLocation, 1, &instance.specular.x);

	glUniform1i(modelColourModeLocation, static_cast<int>(instance.colourMode));
	glUniform4fv(modelColourOverrideLocation, 1, &instance.colour.x);
	glUniform1i(modelHasVertColourLocation, data->hasColours ? 1 : 0);
	glUniform2f(modelMinYMaxYLocation, data->minY, data->maxY);

	Vec3 randSeed = seedFromName(instance.name);
	glUniform3f(modelSeedLocation, randSeed.x, randSeed.y, randSeed.z);

	glUniform1i(modelUseTexturesLocation, instance.useTextures ? 1 : 0);
	if (instance.useTextures) {
		glUniform1i(modelUseTexturesLocation, GL_TRUE);

		int mixLoc = glGetUniformLocation(program, "texMixRatios");
		if (mixLoc >= 0) glUniform4f(mixLoc, instance.textureMixRatio[0], instance.textureMixRatio[1], instance.textureMixRatio[2], instance.textureMixRatio[3]);

		for (size_t i = 0; i < instance.textureNames->size(); ++i) {
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