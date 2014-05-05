#include "FinalPass.header.hlsli"



SamplerState gLinearSampler		:register(s0);

float4 main(vOutFSQ inData) : SV_TARGET
{
	float4 color = gSRVColor.Sample(gLinearSampler, inData.uv);
	float4 normal = gSRVNormal.Sample(gLinearSampler, inData.uv);
	//float4 lighting = gMRTLight.Sample(gLinearSampler, pin.uv);
	//float3 diffuse = lighting.xyz * color;
	float4 light = gLightMap.Sample(gLinearSampler, inData.uv);

	//return color;
	//float4 depth = gDepthCube.Sample(gLinearSampler, color.xyz).r;
	float4 depth = gDepthMap.Sample(gLinearSampler, inData.uv).r;
	/*if (depth.x > 0.0f)
		return float4(1, 0, 0, 1);
		else
		return float4(0, 1, 0, 1);*/
	depth = (depth * 10) - 9.75;
	float4 value;
	value.x = max(0, depth.x);
	value.y = max(0, depth.y);
	value.z = max(0, depth.z);
	value.w = max(0, depth.w);
	return value;
	//return color;// saturate(color) * saturate(light);
	//return saturate(light);
}