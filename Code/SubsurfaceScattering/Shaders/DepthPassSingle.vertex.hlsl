#include "DepthPass.header.hlsli"

float4 main(vertexIn vIn) : SV_POSITION
{
	return mul(float4(vIn.posW, 1.0f), mul(world, mul(view, projection)));
}