#pragma once

#include "graphics/shaderManager.hpp"
#include "graphics/vaoManager.hpp"
#include "core/modelInstance.hpp"
#include "math/mat4.hpp"

class Renderer {
public:
	Renderer();
	~Renderer();

	void initialize(ShaderManager* shaderManager, VAOManager* vaoManager);
	void setProgram(unsigned int program);

	unsigned int getProgram() const { return program; }
	VAOManager* getVAOManager() const { return vaoManager; }

	void updateCameraUniforms(const Vec3& eye, const Mat4& view, const Mat4& projection) const;

	bool drawModel(const ModelInstance& instance, const Mat4& view, const Mat4& projection);

private:
	ShaderManager* shaderManager;
	VAOManager* vaoManager;

	unsigned int program;

	bool modelLighted, modelUseTextures;
	int modelColourModeLocation, modelColourOverrideLocation;
	int modelHasVertColourLocation, modelMinYMaxYLocation, modelSeedLocation;
	int modelLocation, modelViewLocation, modelProjectionLocation;
	int modelInverseTransposeLocation;
	int modelSpecularLocation;
};