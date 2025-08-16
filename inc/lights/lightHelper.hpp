#pragma once

// Helper that returns a light "brightness" level based on attentuation functions, if shader changes, you have to change this too
class LightHelper {
public:
	static const float DEFAULT_ATTEN_CONST;					     // = 0.1f;
	static const float DEFAULT_ATTEN_LINEAR;				     // = 0.1f;
	static const float DEFAULT_ATTEN_QUADRATIC;				   // = 0.1f;
	static const float DEFAULT_AMBIENT_TO_DIFFUSE_RATIO; // = 0.2f;

	// if we haven't hit the desired light level by infiniteDistance, it will return infiniteDistance
	static const unsigned int DEFAULTMAXITERATIONS = 50;
	static const float DEFAULTINFINITEDISTANCE;	         // = 10,000.0f;	
	static const float DEFAULDIFFUSEACCURACYTHRESHOLD;   // = 0.001f;

	float calcApproxDistFromAtten(float targetLightLevel);	
	float calcApproxDistFromAtten(float targetLightLevel, float accuracy);	
	float calcApproxDistFromAtten(float targetLightLevel, float accuracy, float infiniteDistance, 
		float constAttenuation, float linearAttenuation, float quadraticAttenuation,
		unsigned int maxIterations = DEFAULTMAXITERATIONS);

	static const float DEFAULTZEROTHRESHOLD;	           // = 0.0001f;
	 
	// If value gets below the "zeroThreshold", will be treated as zero
	float calcDiffuseFromAttenByDistance(float distance, 
		float constAttenuation, float linearAttenuation, float quadraticAttenuation, 
		float zeroThreshold = DEFAULTZEROTHRESHOLD);
};