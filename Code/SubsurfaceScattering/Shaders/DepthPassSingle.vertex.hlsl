#include "DepthPass.header.hlsli"

float4 main(vertexIn vIn) : SV_POSITION
{
	float depth = length(mul(float4(vIn.posW, 1.0f), world).xyz - posRange.xyz);
	
	//return float4(vIn.posW, 1.0f);
	return mul(float4(vIn.posW, 1.0f), mul(world, mul(view, projection)));

}