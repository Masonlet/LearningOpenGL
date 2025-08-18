#include <glad/glad.h>

#include "graphics/renderer.hpp"

Renderer::Renderer(ShaderManager& shaderManagerIn, MeshManager& meshManagerIn, TextureManager& textureManagerIn)
	: shaderManager(shaderManagerIn), meshManager(meshManagerIn), textureManager(textureManagerIn),
	  program(0),
	  modelLocation(-1), modelViewLocation(-1), modelProjectionLocation(-1),
		modelInverseTransposeLocation(-1), modelSpecularLocation(-1),
		modelLighted(false), modelUseTextures(false),
		modelColourModeLocation(-1), modelColourOverrideLocation(-1),
		modelHasVertColourLocation(-1), modelMinYMaxYLocation(-1),
		modelSeedLocation(-1) {
}

Renderer::~Renderer() {}

void Renderer::setProgram(unsigned int program) {
	this->program = program;
	glUseProgram(program);

	glUniform1i(glGetUniformLocation(program, "textSampler2D_00"), 0);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_01"), 1);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_02"), 2);
	glUniform1i(glGetUniformLocation(program, "textSampler2D_03"), 3);

	modelLighted = glGetUniformLocation(program, "bLighted");
	modelLocation = glGetUniformLocation(program, "mModel");
	modelViewLocation = glGetUniformLocation(program, "mView");
	modelProjectionLocation = glGetUniformLocation(program, "mProj");
	modelInverseTransposeLocation = glGetUniformLocation(program, "mModel_InverseTranpose");
	modelSpecularLocation = glGetUniformLocation(program, "vertSpecular");
	modelUseTextures = glGetUniformLocation(program, "bUseTextures");
	modelColourModeLocation = glGetUniformLocation(program, "colourMode");
	modelColourOverrideLocation = glGetUniformLocation(program, "colourOverride");
	modelHasVertColourLocation = glGetUniformLocation(program, "hasVertexColour");
	modelMinYMaxYLocation = glGetUniformLocation(program, "yMin_yMax");
	modelSeedLocation = glGetUniformLocation(program, "seed");
}

void Renderer::updateCameraUniforms(const Vec3& eye, const Mat4& view, const Mat4& projection) const {
	glUniform3f(glGetUniformLocation(program, "eyeLocation"), eye.x, eye.y, eye.z);
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
bool Renderer::drawModel(const ModelData& instance, const Mat4& view, const Mat4& projection) {
	if (!instance.isVisible) return true;

	const MeshData* info{};
	if (!meshManager.findMesh(instance.meshPath, info)) {
		fprintf(stderr, "[Renderer ERROR] Could not find mesh: %s\n", instance.meshPath.c_str());
		return false;
	}
	 
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, instance.modelMatrix.data);
	Mat4 modelIT = instance.modelMatrix.inverse().transpose();
	glUniformMatrix4fv(modelInverseTransposeLocation, 1, GL_FALSE, modelIT.data);
	glUniform4fv(modelSpecularLocation, 1, &instance.specular.x);

	glUniform1i(modelColourModeLocation, static_cast<int>(instance.colourMode));
	glUniform4fv(modelColourOverrideLocation, 1, &instance.colour.x);
	glUniform1i(modelHasVertColourLocation, info->hasColours ? 1 : 0);
	glUniform2f(modelMinYMaxYLocation, info->minY, info->maxY);

	Vec3 randSeed = seedFromName(instance.name);
	glUniform3f(modelSeedLocation, randSeed.x, randSeed.y, randSeed.z);

	glUniform1i(modelUseTextures, instance.useTextures ? 1 : 0);
	if (instance.useTextures) {
		glUniform1i(modelUseTextures, GL_TRUE);

		if (GLint mixLoc = glGetUniformLocation(program, "texMixRatios"); mixLoc >= 0) {
			glUniform4f(mixLoc,
				instance.textureMixRatio[0],
				instance.textureMixRatio[1],
				instance.textureMixRatio[2],
				instance.textureMixRatio[3]);
		}

		for (size_t i = 0; i < 4; ++i) {
			const std::string& name = instance.textureNames[i];
			if (name.empty()) continue;

			int textureID = textureManager.getTextureIDFromName(name);
			if(textureID == 0) {
				fprintf(stderr, "[Renderer ERROR] Could not find texture: %s\n", name.c_str());
				return false;
			}

			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureID);
		}
	}

	glUniform1i(modelLighted, instance.isLighted ? 1 : 0);

	if (instance.colour.w < 1.0f)	glDepthMask(GL_FALSE);
	glBindVertexArray(info->VAOID);
	glDrawElements(GL_TRIANGLES, info->numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (instance.colour.w < 1.0f) glDepthMask(GL_TRUE);

	return true;
}