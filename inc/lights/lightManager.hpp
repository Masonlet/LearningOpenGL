#pragma once
#include "math/vec4.hpp"

#include <string>

class Light {
public:
	Light();
	Vec4 position;	// xyz = position, ignoring w = TBD
	Vec4 diffuse;   // rgb = diffuse colour, w = intensity
	Vec4 specular;	// rgb = highlight colour, w = power
	Vec4 atten;		  // x = constant, y = linear, z = quadratic, w = cutoff distance
	Vec4 direction;	// xyz = direction (spot/directional), w = unused
	Vec4 param1;	  // x = lightType (0 = Point, 1 = Spot, 2 = Directional), y = inner angle (spot), z = outer angle (spot), w = TBD
	Vec4 param2;	  // x = enabled (0 = off, 1 = on), yzw = TBD

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
	void UpdateShaderUniforms(int shaderProgram) const;

	static const int NUMBEROFLIGHTS = 50;
	Light theLights[NUMBEROFLIGHTS];
	std::string lightNames[NUMBEROFLIGHTS];

	Light* getLightByName(const std::string& name);
	const std::string& getLightName(int index) const { return lightNames[index]; }
};
