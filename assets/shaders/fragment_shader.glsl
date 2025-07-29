#version 330

const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 50;

struct Light {
	vec4 position;	// xyz = position, ignoring w = TBD
	vec4 diffuse;	// rgb = diffuse colour, w = intensity
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = cutoff distance
	vec4 direction;	// xyz = direction (spot/directional), w = unused
	vec4 param1;	// x = lightType (0 = Point, 1 = Spot, 2 = Directional), y = inner angle (spot), z = outer angle (spot), w = TBD
	vec4 param2;	// x = enabled (0 = off, 1 = on), yzw = TBD
};

uniform Light theLights[NUMBEROFLIGHTS];
uniform vec3 eyeLocation;
uniform vec4 vertSpecular;

in vec4 vertColor;
in vec4 vertNormal;
in vec4 vertWorldPosition;

out vec4 pixelColour;
		
vec4 calculateLightContrib( vec3 vertexMaterialColour, vec3 vertexNormal, vec3 vertexWorldPos, vec4 vertexSpecular ) {
	vec3 norm = normalize(vertexNormal);
	vec4 finalObjectColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	for (int index = 0; index < NUMBEROFLIGHTS; index++) {	
		if (theLights[index].param2.x == 0.0f) continue;
		
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		// Simulate sunlight. There's ONLY direction, no position -Almost always, there's only 1 of these in a scene, Cheapest light to calculate. 
		if (intLightType == DIRECTIONAL_LIGHT_TYPE)	{
			float dotProduct = dot(-theLights[index].direction.xyz, normalize(norm.xyz));	
			dotProduct = max(0.0f, dotProduct);

			vec3 lightContrib = theLights[index].diffuse.rgb * dotProduct;

			finalObjectColour.rgb += ( vertexMaterialColour.rgb * 
			                           theLights[index].diffuse.rgb * 
									   lightContrib /*+ 
									   (materialSpecular.rgb * lightSpecularContrib.rgb);*/);

			continue;		
		}
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertexWorldPos.xyz;	
		float distanceToLight = length(vLightToVertex);		
		vec3 lightVector = normalize(vLightToVertex);	
		
		float dotProduct = dot(lightVector, vertexNormal.xyz);	 
		dotProduct = max(0.0f, dotProduct);	

		// Diffuse
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
		
		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);

		// Get eye or view vector The location of the vertex in the world to your eye
		vec3 eyeVector = normalize(eyeLocation.xyz - vertexWorldPos.xyz);

		// To simplify, we are NOT using the light specular value, just the object’s.
		vec3 reflectVector = reflect(-lightVector, normalize(norm.xyz));
		float objectSpecularPower = vertexSpecular.w; 
		lightSpecularContrib = pow(max(0.0f, dot(eyeVector, reflectVector)), objectSpecularPower ) * vertexSpecular.rgb; //* theLights[lightIndex].Specular.rgb
					   
		float attenuation = 1.0f / (theLights[index].atten.x + 
									theLights[index].atten.y * distanceToLight + 
									theLights[index].atten.z * distanceToLight*distanceToLight);  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;
		
		if (intLightType == SPOT_LIGHT_TYPE) {	
			vec3 vertexToLight = vertexWorldPos.xyz - theLights[index].position.xyz;
			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle = dot(vertexToLight.xyz, theLights[index].direction.xyz);
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if (currentLightRayAngle < outerConeAngleCos) {
				// Nope, it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if (currentLightRayAngle < innerConeAngleCos) {
				// Angle is between the inner and outer cone (called the penumbra of the spot light)
				// This blends the brightness from,	full brightness near the inner cone, to black near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}		
		}
				
		finalObjectColour.rgb += (vertexMaterialColour.rgb * lightDiffuseContrib.rgb) + (vertexSpecular.rgb * lightSpecularContrib.rgb );
	}
	finalObjectColour.a = 1.0f;
	
	return finalObjectColour;
}

void main() {
	pixelColour = vec4(vertColor);
	
	vec4 lightContrib = calculateLightContrib(vertColor.rgb, vertNormal.xyz, vertWorldPosition.xyz, vertSpecular);
	
	pixelColour.rgb = lightContrib.rgb;
};