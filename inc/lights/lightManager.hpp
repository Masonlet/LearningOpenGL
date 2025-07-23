#pragma once
#include "math/vec4.hpp"

#include <string>

class Light {
public:
	Light();
	Vec4 position;	// xyz, ignoring w (4th parameter)	
	Vec4 diffuse;
	Vec4 specular;	// rgb = highlight colour, w = power
	Vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	Vec4 direction;	// Spot, directional lights, ignoring w (4th parameter)	
	Vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	// 0 = pointlight
	// 1 = spot light
	// 2 = directional light
	Vec4 param2;	// x = 0 for off, 1 for on

	int position_UL = -1;
	int diffuse_UL = -1;
	int specular_UL = -1;
	int atten_UL = -1;
	int direction_UL = -1;
	int param1_UL = -1;
	int param2_UL = -1;
};


class LightManager {
public:
	LightManager() {};

	void GetUniformLocations(int shaderProgram);
	void UpdateShaderUniforms(int shaderProgram);

	static const int NUMBEROFLIGHTS = 50;
	Light theLights[NUMBEROFLIGHTS];
	std::string lightNames[NUMBEROFLIGHTS];

	Light* getLightByName(const std::string& name);
	const std::string& getLightName(int index) const { return lightNames[index]; }
};
