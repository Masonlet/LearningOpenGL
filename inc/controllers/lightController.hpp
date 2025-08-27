#pragma once

#include "controller.hpp"
#include "objects/light.hpp"
#include <cstddef>
#include <map>

class LightController : public Controller<Light> {
public:

	unsigned int currentLight{ 0 };
	inline void incrementLight(std::size_t lightCount) { if (lightCount != 0 && static_cast<size_t>(currentLight + 1) < lightCount) currentLight++; }
	inline void decrementLight() { if (currentLight != 0) currentLight--; }

	void update(Light& light, const InputManager& input, float deltaTime) override {};
	void updateLightLocations(std::map<std::string, Light>& lights, int shaderProgram);
	void updateLightUniforms(std::map<std::string, Light>& lights, int shaderProgram);
};