#include "FinalPass.header.hlsli"



SamplerState gLinearSampler		:register(s0);


float4 main(vOutFSQ inData) : SV_TARGET0
{
	float4 color = gSRVColor.Sample(gLinearSampler, inData.uv);
	float4 light = gLightMap.Sample(gLinearSampler, inData.uv);
	float4 depth = gDepthMap.Sample(gLinearSampler, inData.uv).r;
<<<<<<< HEAD
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
	return depth;
	//return color;// saturate(color) * saturate(light);
	//return saturate(light);
=======
	
	return saturate(color) * saturate(light);
>>>>>>> 1dff155c5f61c13ed99df508c39256fef11d8e68
}