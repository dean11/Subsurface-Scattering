#include "ShaderData.header.hlsl"

Texture2D gSRVColor				:register(t0);
Texture2D gSRVLight				:register(t1);

SamplerState gLinearSampler		:register(s0);

float4 main(vOutFSQ inData) : SV_TARGET
{
	float3 color = gSRVColor.Sample(gLinearSampler, inData.uv).rgb;
	//float4 lighting = gMRTLight.Sample(gLinearSampler, pin.uv);
	//float3 diffuse = lighting.xyz * color;

	return float4(color, 1.0f);
}