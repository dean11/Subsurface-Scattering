#include "ShaderData.header.hlsl"

Texture2D gSRVNormal			:register(t0);
Texture2D gSRVColor				:register(t1);

SamplerState gLinearSampler		:register(s0);

float4 main(vOutFSQ inData) : SV_TARGET
{
	float3 color = gSRVColor.Sample(gLinearSampler, inData.uv).rgb;
	//float4 lighting = gMRTLight.Sample(gLinearSampler, pin.uv);
	//float3 diffuse = lighting.xyz * color;

	//return float4(1.0, 0.1, 0.5, 1.0f);
	return float4(color, 1.0f);
}