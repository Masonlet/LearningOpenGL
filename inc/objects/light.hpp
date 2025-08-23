#pragma once

#include "math/vec4.hpp"

class Light {
public:
	std::string name;

	Vec3 pos        { 0.0f };	                    // xyz = position
	Vec4 diffuse    { 1.0f };                     // rgb = diffuse colour, w = intensity
	Vec4 attenuation{ 0.0f, 0.01f, 0.01f, 1.0f }; // x = constant, y = linear, z = quadratic, w = cutoff distance
	Vec4 direction  { 0.0f, 0.0f, 0.0f, 1.0f };	  // xyz = direction (spot/directional), w = unused 
	unsigned int type{ 0 };											  // 0 = Point, 1 = Spot, 2 = Directional
	Vec3 param1     { 0.0f };	                    // x = inner angle (spot), y = outer angle (spot), z = TBD
	bool enabled{ true };                         // Light enabled

	int position_UL   { -1 };
	int diffuse_UL    { -1 };
	int attenuation_UL{ -1 };
	int direction_UL  { -1 };
	int param1_UL     { -1 };
	int param2_UL     { -1 };
};
