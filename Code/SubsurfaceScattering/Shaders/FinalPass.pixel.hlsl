#include "FinalPass.header.hlsli"



SamplerState gLinearSampler		:register(s0);

float4 main(vOutFSQ inData) : SV_TARGET
{
	float4 color = gSRVColor.Sample(gLinearSampler, inData.uv);
	//float4 lighting = gMRTLight.Sample(gLinearSampler, pin.uv);
	//float3 diffuse = lighting.xyz * color;
	float4 light = gLightMap.Sample(gLinearSampler, inData.uv);
	//return float4(1.0, 0.1, 0.5, 1.0f);
	//return float4(color, 1.0f);
	
	return saturate(color) * saturate(light);
	//return saturate(light);
}