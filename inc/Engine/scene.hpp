#pragma once

#include <string>
#include "engine.hpp"


class Scene {
public:
	virtual ~Scene() = default;

	virtual void Load(Engine& engine) = 0;

	virtual bool isLoaded() const = 0;
};

Scene* createSceneFromName(const std::string& name);