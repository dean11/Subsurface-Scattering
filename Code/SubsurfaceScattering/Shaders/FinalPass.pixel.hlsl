#include "FinalPass.header.hlsli"

float4 main(vOutFSQ inData) : SV_TARGET0
{
	float4 color = gSRVColor.Sample(gLinearSampler, inData.uv);
	float4 light = gLightMap.Sample(gLinearSampler, inData.uv);

	//return saturate(color) * saturate(light);
	return float4((color.xyz * light.xyz), 1.0f);
	return color;
}