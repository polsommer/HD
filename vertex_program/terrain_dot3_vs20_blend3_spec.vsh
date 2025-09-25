//hlsl vs_2_0 vs_3_0

#include "vertex_program/include/vertex_shader_constants.inc"
#include "vertex_program/include/functions.inc"

struct InputVertex
{
	float4  position              : POSITION0  : register(v0);
	float4  normal                : NORMAL0    : register(v3);
	float4  color                 : COLOR0     : register(v5);
	float2  textureCoordinateSet0 : TEXCOORD0  : register(v7);
	float2  textureCoordinateSet1 : TEXCOORD1  : register(v8);
	float2  textureCoordinateSet2 : TEXCOORD2  : register(v9);
	float2  textureCoordinateSet3 : TEXCOORD3  : register(v10);
};

struct OutputVertex
{
	float4  position              : POSITION0;
	float4  diffuse               : COLOR0;
	float4  dot3Color             : COLOR1;
	float   fog                   : FOG;
	float3  lightDirection_t      : TEXCOORD0;
	float2  textureCoordinateSet1 : TEXCOORD1;
	float2  textureCoordinateSet2 : TEXCOORD2;
	float2  textureCoordinateSet3 : TEXCOORD3;
	float2  textureCoordinateSet4 : TEXCOORD4;
	float3 halfAngle_t            : TEXCOORD5;
	float3 emissive               : TEXCOORD6;
	float3 eyeVector_t            : TEXCOORD7;
};

OutputVertex main(InputVertex inputVertex)
{
	OutputVertex outputVertex;

	// transform vertex
	outputVertex.position = transform3d(inputVertex.position);

	// calculate fog
	outputVertex.fog = calculateFog(inputVertex.position);

	// calculate lighting
	DiffuseSpecular diffuseSpecular = calculateDiffuseSpecularTerrainLighting(true, inputVertex.position, inputVertex.normal);
	outputVertex.diffuse  = lightData.ambient.ambientColor + diffuseSpecular.diffuse;
	outputVertex.diffuse  = min(outputVertex.diffuse, 1.0) * inputVertex.color;

	//Get view direction and compute half-angle
	float3 h = lightData.dot3[0].direction_o + normalize(lightData.dot3[0].cameraPosition_o - inputVertex.position);
	outputVertex.halfAngle_t = transformTerrainDot3(h, inputVertex.normal);

	// store dot3 light modulated by vertex color
	outputVertex.dot3Color = inputVertex.color;

	// copy texture coordinates
	outputVertex.lightDirection_t = transformTerrainDot3LightDirection(inputVertex.normal);
	outputVertex.textureCoordinateSet1 = inputVertex.textureCoordinateSet0;
	outputVertex.textureCoordinateSet2 = inputVertex.textureCoordinateSet1;
	outputVertex.textureCoordinateSet3 = inputVertex.textureCoordinateSet2;
	outputVertex.textureCoordinateSet4 = inputVertex.textureCoordinateSet3;

	outputVertex.emissive = intensity(material.emissiveColor);

	outputVertex.eyeVector_t  = transformTerrainDot3(normalize(lightData.dot3[0].cameraPosition_o - inputVertex.position), inputVertex.normal);

	return outputVertex;
}
