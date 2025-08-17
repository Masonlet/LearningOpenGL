#include "lights/lightHelper.hpp"

const float LightHelper::DEFAULT_ATTEN_CONST = 0.1f;
const float LightHelper::DEFAULT_ATTEN_LINEAR = 0.1f;
const float LightHelper::DEFAULT_ATTEN_QUADRATIC = 0.1f;
const float LightHelper::DEFAULT_AMBIENT_TO_DIFFUSE_RATIO = 0.8f;
const float LightHelper::DEFAULTINFINITEDISTANCE = 10000.0f;
const float LightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD = 0.001f;

float LightHelper::calcApproxDistFromAtten(float targetLightLevel) {
	return this->calcApproxDistFromAtten(targetLightLevel, LightHelper::DEFAULDIFFUSEACCURACYTHRESHOLD);
}

float LightHelper::calcApproxDistFromAtten(float targetLightLevel, float accuracy) {
	return this->calcApproxDistFromAtten(targetLightLevel, accuracy,
		LightHelper::DEFAULT_ATTEN_CONST, LightHelper::DEFAULT_ATTEN_LINEAR, LightHelper::DEFAULT_ATTEN_QUADRATIC,
		LightHelper::DEFAULTINFINITEDISTANCE, LightHelper::DEFAULTMAXITERATIONS);
}

float LightHelper::calcApproxDistFromAtten(float targetLightLevel, float accuracy, float infiniteDistance,
	float constAttenuation, float linearAttenuation, float quadraticAttenuation, unsigned int maxIterations) {
	// See if the accuracy being set it too big for the targetLightLevel, unless targetLightLevel is actually zero (0.0f)
	// If it's actually zero, then adjusting the accuracy to a tenth of zero would give zero, and we would max out the iterations
	if (targetLightLevel != 0.0f) 
		if ((accuracy * 10.0f) >= targetLightLevel * 10.0f)	
			accuracy = targetLightLevel / 10.0f;

	float targetLightLevelLow = targetLightLevel - accuracy;
	float targetLightLevelHigh = targetLightLevel + accuracy;

	// See if we're getting a value at infinite. i.e. at 'infinite distance', is the light level too high already
	if (this->calcDiffuseFromAttenByDistance(LightHelper::DEFAULTINFINITEDISTANCE, constAttenuation, linearAttenuation, quadraticAttenuation, accuracy) > targetLightLevelHigh) 	
		return LightHelper::DEFAULTINFINITEDISTANCE;

	float distanceGuessLow = 0.0f;
	float distanceGuessHigh = LightHelper::DEFAULTINFINITEDISTANCE;

	unsigned int iterationCount = 0;
	while (iterationCount < maxIterations) {
		// Pick a distance between the high and low
		float curDistanceGuess = ((distanceGuessHigh - distanceGuessLow) / 2.0f) + distanceGuessLow;

		// Could be three possibilities: too low, too high, or in between
		float curDiffuseAtGuessDistance = this->calcDiffuseFromAttenByDistance(curDistanceGuess, constAttenuation, linearAttenuation, quadraticAttenuation, DEFAULTZEROTHRESHOLD);
		if      (curDiffuseAtGuessDistance < targetLightLevelLow ) distanceGuessHigh = curDistanceGuess; // Light is too dark, so distance is to HIGH.
		else if (curDiffuseAtGuessDistance > targetLightLevelHigh) distanceGuessLow = curDistanceGuess; // Light is too bright, so distance is to LOW.
		else                                                       return curDistanceGuess;

		iterationCount++;
	}

	// If we are here, then we ran out of iterations. Pick a distance between the low and high
	return (distanceGuessHigh - distanceGuessLow) / 2.0f;
}

const float LightHelper::DEFAULTZEROTHRESHOLD = 0.0001f;
float LightHelper::calcDiffuseFromAttenByDistance(
	float distance,
	float constAttenuation, float linearAttenuation, float quadraticAttenuation,
	float zeroThreshold) {
	float diffuse = 1.0f;	// Assume full brightness
	float denominator = constAttenuation + linearAttenuation * distance + quadraticAttenuation * distance * distance;

	if (denominator > zeroThreshold){
		diffuse *= 1.0f / denominator;
		if (diffuse > 1.0f) return 1.0f;
	} 
	return diffuse;
}
