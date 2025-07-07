#pragma once

class LightHelper {
public:
	static const float DEFAULT_ATTEN_CONST;					// = 0.1f;
	static const float DEFAULT_ATTEN_LINEAR;				// = 0.1f;
	static const float DEFAULT_ATTEN_QUADRATIC;				// = 0.1f;
	static const float DEFAULT_AMBIENT_TO_DIFFUSE_RATIO;	// = 0.2f;
	
	//Helper that returns a light "brightness" level based on attentuation functions
	//If shader changes, you have to change this too

	// infiniteDistance is where there's no point in continuing the calculation. 
	//	i.e. if we still haven't hit the desired light level by infiniteDistance, it will reutrn infiniteDistance
	static const unsigned int DEFAULTMAXITERATIONS = 50;
	static const float DEFAULTINFINITEDISTANCE;	// = 10,000.0f;	
	static const float DEFAULDIFFUSEACCURACYTHRESHOLD; // = 0.001f;
	float calcApproxDistFromAtten(float targetLightLevel);	// Uses the defaults
	float calcApproxDistFromAtten(float targetLightLevel, float accuracy);	// Uses the defaults
	
	float calcApproxDistFromAtten(float targetLightLevel, float accuracy, float infiniteDistance, 
		float constAttenuation, float linearAttenuation, float quadraticAttenuation,
		unsigned int maxIterations = DEFAULTMAXITERATIONS /*= 50*/);

	static const float DEFAULTZEROTHRESHOLD;	// = 0.0001f;

	// If value gets below the "zeroThreshold", will be treated as zero
	//float calcAttenFromDist(float distance, float zeroThreshold = DEFAULTZEROTHRESHOLD);
	float calcDiffuseFromAttenByDistance(float distance, 
		float constAttenuation, float linearAttenuation, float quadraticAttenuation, 
		float zeroThreshold = DEFAULTZEROTHRESHOLD);
};