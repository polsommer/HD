//hlsl vs_1_1 vs_2_0

#define textureCoordinateSetDOT3	textureCoordinateSet0
#define textureCoordinateSetNRML	textureCoordinateSet1
#define DECLARE_textureCoordinateSets	\
	float4 textureCoordinateSet0 : TEXCOORD0 : register(v7); \
	float2 textureCoordinateSet1 : TEXCOORD1 : register(v8);

#include "vertex_program/include/vertex_shader_constants.inc"
#include "vertex_program/include/functions.inc"

struct InputVertex
{
	float4  position              : POSITION0  : register(v0);
	float4  normal                : NORMAL0    : register(v3);
	DECLARE_textureCoordinateSets
};

struct OutputVertex
{
	float4  position               : POSITION0;
	float4  diffuse                : COLOR0;
	float   fog                    : FOG;
	float3  textureCoordinateSet0  : TEXCOORD0;
	float2  textureCoordinateSet1  : TEXCOORD1;
};

OutputVertex main(InputVertex inputVertex)
{
	OutputVertex outputVertex;

	// transform vertex
	outputVertex.position = transform3d(inputVertex.position);

	// calculate fog
	outputVertex.fog = calculateFog(inputVertex.position);

	// calculate lighting
	outputVertex.diffuse =  lightData.ambient.ambientColor;
	outputVertex.diffuse += calculateDiffuseLighting(true, inputVertex.position, inputVertex.normal);

	// setup for per-pixel lighting
	// pack the normal into 0..1 because ps1.1 clamps to 0..1
	outputVertex.textureCoordinateSet0 = reverseSignAndBias(transformDot3LightDirection(inputVertex.normal, inputVertex.textureCoordinateSetDOT3));
	outputVertex.textureCoordinateSet1 = inputVertex.textureCoordinateSetNRML;

	return outputVertex;
}

