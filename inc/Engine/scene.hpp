#pragma once

#include "model.hpp"

enum class RenderMode {
	basic2D,
	basic3D
};

class Scene {
public:
	virtual ~Scene() = default;

	virtual void Load(Model* models) = 0;

	virtual bool isLoaded() const { return false; }
	virtual unsigned int objectCount() const { return 0; }
	virtual RenderMode renderMode() const { return RenderMode::basic3D; }
};

Scene* createSceneFromName(const unsigned char* name);