#pragma once

#include "graphics/shaderManager.hpp"
#include "graphics/meshManager.hpp"
#include "graphics/textureManager.hpp"
#include "models/modelData.hpp"
#include "math/mat4.hpp"

class Renderer {
public:
	Renderer(ShaderManager& shaderManagerIn, MeshManager& meshManagerIn, TextureManager& textureManagerIn);
	~Renderer();

	void setProgram(unsigned int program);

	unsigned int getProgram() const { return program; }
	int getIsSkyboxLocation() const { return modelIsSkyboxLocation; }
	int getSkyboxTextureLocation() const { return modelSkyboxTextureLocation; }

	void updateCameraUniforms(const Vec3& eye, const Mat4& view, const Mat4& projection) const;

	bool drawModel(const ModelData& instance, const Mat4& view, const Mat4& projection);

private:
	ShaderManager& shaderManager;
	MeshManager& meshManager;
	TextureManager& textureManager;

	unsigned int program;

	int modelLightedLocation, modelUseTexturesLocation, modelIsSkyboxLocation;
	int modelColourModeLocation, modelColourOverrideLocation;
	int modelHasVertColourLocation, modelMinYMaxYLocation, modelSeedLocation;
	int modelLocation, modelViewLocation, modelProjectionLocation;
	int modelInverseTransposeLocation;
	int modelSpecularLocation;
	int modelSkyboxTextureLocation;
};