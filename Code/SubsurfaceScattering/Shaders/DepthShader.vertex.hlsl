#include "DepthShader.header.hlsli"

float4 main(vertexIn vIn) : SV_POSITION
{
	float4 pos = mul(float4(vIn.posW, 1.0f), mul(world, mul(lightView, lightProjection)));
	pos.z *= pos.w; // For linear positions
	return pos;
}