#pragma once

#include "model.hpp"

class Scene {
public:
	virtual ~Scene() = default;
	virtual void Load(Model* models) = 0;
	virtual bool isLoaded() const { return false; }
	virtual unsigned int objectCount() const { return 0; }
};

Scene* createSceneFromName(const char* name);