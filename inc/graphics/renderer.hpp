#pragma once

#include "graphics/shaderManager.hpp"
#include "graphics/meshManager.hpp"
#include "objects/modelData.hpp"
#include "scene/sceneManager.hpp"
#include "math/mat4.hpp"

class Renderer {
public:
	void setProgram(unsigned int program);

	unsigned int getProgram() const { return program; }
	int getIsSkyboxLocation() const { return modelIsSkyboxLocation; }
	int getSkyboxTextureLocation() const { return modelSkyboxTextureLocation; }

	void updateCameraUniforms(const Vec3& eye, const Mat4& view, const Mat4& projection) const;

	bool drawModel(MeshManager&, SceneManager& sceneManager, const ModelData& instance, const Mat4& view, const Mat4& projection) const;

private:
	unsigned int program{ 0 };

	int modelLightedLocation{ -1 }, modelUseTexturesLocation{ -1 }, modelIsSkyboxLocation{ -1 };
	int modelColourModeLocation{ -1 }, modelColourOverrideLocation{ -1 };
	int modelHasVertColourLocation{ -1 }, modelMinYMaxYLocation{ -1 }, modelSeedLocation{ -1 };
	int modelLocation{ -1 }, modelViewLocation{ -1 }, modelProjectionLocation{ -1 };
	int modelInverseTransposeLocation{ -1 };
	int modelSpecularLocation{ -1 };
	int modelSkyboxTextureLocation{ -1 };
};