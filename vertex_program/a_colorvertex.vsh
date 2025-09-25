//hlsl vs_1_1 vs_2_0

#define textureCoordinateSetMAIN	textureCoordinateSet0
#define DECLARE_textureCoordinateSets	\
	float2 textureCoordinateSet0 : TEXCOORD0 : register(v7);

#include "vertex_program/include/vertex_shader_constants.inc"
#include "vertex_program/include/functions.inc"

struct InputVertex
{
	float4  position              : POSITION0  : register(v0);
	float4  normal                : NORMAL0    : register(v3);
	float4  diffuse               : COLOR0     : register(v5);
	DECLARE_textureCoordinateSets
};

struct OutputVertex
{
	float4  position              : POSITION0;
	float4  diffuse               : COLOR0;
	float   fog                   : FOG;
	float2  textureCoordinateSet0 : TEXCOORD0;
};

OutputVertex main(InputVertex inputVertex)
{
	OutputVertex outputVertex;

	// transform vertex
	outputVertex.position = transform3d(inputVertex.position);

	// calculate fog
	outputVertex.fog = calculateFog(inputVertex.position);

	// calculate lighting
	outputVertex.diffuse = inputVertex.diffuse;
	outputVertex.diffuse.rgb *= (lightData.ambient.ambientColor + calculateDiffuseLighting(false, inputVertex.position, inputVertex.normal));

	// copy texture coordinates
	outputVertex.textureCoordinateSet0 = inputVertex.textureCoordinateSetMAIN;

	return outputVertex;
}
