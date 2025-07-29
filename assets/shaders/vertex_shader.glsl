#version 330

uniform mat4 mProj;
uniform mat4 mView;
uniform mat4 mModel;
uniform mat4 mModel_InverseTranpose;

uniform bool bUseOverrideColor;
uniform vec3 colorOverride;

in vec4 vCol;
in vec4 vPos;
in vec4 vNorm;

out vec4 vertColor;
out vec4 vertNormal;
out vec4 vertWorldPosition;

void main() {
    gl_Position = (mProj * mView * mModel) * vec4(vPos.xyz, 1.0f);

	vertWorldPosition = mModel * vec4(vPos.xyz, 1.0f);
	vertNormal = mModel_InverseTranpose * vec4(vNorm.xyz, 1.0f);
	vertNormal.xyz = normalize(vertNormal.xyz);

	if (bUseOverrideColor) vertColor = vec4(colorOverride.rgb, 1.0f);
	else vertColor = vec4(vCol.rgb, 1.0f);
};