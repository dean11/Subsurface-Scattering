#include "FinalPass.header.hlsli"



SamplerState gLinearSampler		:register(s0);


float4 main(vOutFSQ inData) : SV_TARGET0
{
	float4 color = gSRVColor.Sample(gLinearSampler, inData.uv);
	float4 light = gLightMap.Sample(gLinearSampler, inData.uv);
	float4 depth = gDepthMap.Sample(gLinearSampler, inData.uv).r;
	
	return saturate(color) * saturate(light);
}