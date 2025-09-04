#pragma once

#include "controller.hpp"
#include "starletparsers/scene/light.hpp"
#include <cstddef>
#include <map>
#include <string>

struct LightController : public Controller<Light> {
	void updateLightLocations(std::map<std::string, Light>& lights, int shaderProgram);
	void updateLightUniforms(std::map<std::string, Light>& lights, int shaderProgram);

	void update(Light&, const InputManager&, float) override {}
};