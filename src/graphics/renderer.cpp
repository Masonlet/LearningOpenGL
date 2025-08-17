#include <glad/glad.h>

#include "graphics/renderer.hpp"

Renderer::Renderer() : shaderManager(nullptr), vaoManager(nullptr), program(0),
											 modelLocation(-1), modelViewLocation(-1), modelProjectionLocation(-1),
											 modelInverseTransposeLocation(-1), 
											 useOverrideColourLocation(-1), colourOverrideLocation(-1),
											 modelSpecularLocation(-1){}

Renderer::~Renderer() {}

void Renderer::initialize(ShaderManager* shaderManager, VAOManager* vaoManager) {
	this->shaderManager = shaderManager;
	this->vaoManager = vaoManager;
}

void Renderer::setProgram(unsigned int program) {
	this->program = program;
	glUseProgram(program);

	modelLighted = glGetUniformLocation(program, "bLighted");
	modelLocation = glGetUniformLocation(program, "mModel");
	modelViewLocation = glGetUniformLocation(program, "mView");
	modelProjectionLocation = glGetUniformLocation(program, "mProj");
	modelInverseTransposeLocation = glGetUniformLocation(program, "mModel_InverseTranpose");
	modelSpecularLocation = glGetUniformLocation(program, "vertSpecular");

	useOverrideColourLocation = glGetUniformLocation(program, "bUseOverrideColour");
	colourOverrideLocation = glGetUniformLocation(program, "colourOverride");
	modelUseTextures = glGetUniformLocation(program, "bUseTextures");
}

void Renderer::updateCameraUniforms(const Vec3& eye, const Mat4& view, const Mat4& projection) const {
  glUniform3f(glGetUniformLocation(program, "eyeLocation"), eye.x, eye.y, eye.z);
  glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, view.ptr());
  glUniformMatrix4fv(modelProjectionLocation, 1, GL_FALSE, projection.ptr());
}

bool Renderer::drawModel(const ModelInstance& instance, const Mat4& view, const Mat4& projection) {
	if (!instance.isVisible) return true;

	const ModelDrawInfo* info;
	if (!vaoManager->FindDrawInfoByModelName(instance.meshPath, info)) {
		fprintf(stderr, "[Renderer ERROR] Could not find mesh: %s\n", instance.meshPath.c_str());
		return false;
	}

	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, instance.modelMatrix.data);
	Mat4 modelIT = instance.modelMatrix.inverse().transpose();
	glUniformMatrix4fv(modelInverseTransposeLocation, 1, GL_FALSE, modelIT.data);
	glUniform4fv(modelSpecularLocation, 1, &instance.specular.x);

	if (instance.colourMode == ColourMode::Solid) {
		glUniform1f(useOverrideColourLocation, 1.0f);
		glUniform4fv(colourOverrideLocation, 1, &instance.colour.x);
	} 
	else glUniform1f(useOverrideColourLocation, 0.0f);

	if(instance.useTextures) glUniform1f(modelUseTextures, (GLfloat)GL_TRUE);
	else										 glUniform1f(modelUseTextures, (GLfloat)GL_FALSE);

	if (instance.isLighted) glUniform1f(modelLighted, (GLfloat)GL_TRUE);
	else									  glUniform1f(modelLighted, (GLfloat)GL_FALSE);

	if (instance.colour.w < 1.0f)	glDepthMask(GL_FALSE);
	glBindVertexArray(info->VAOID);
	glDrawElements(GL_TRIANGLES, info->numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	if (instance.colour.w < 1.0f) glDepthMask(GL_TRUE);

	return true;
}