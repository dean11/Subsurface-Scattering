
#include "ShaderData.header.hlsl"

//GEOMETRY PIXEL SHADER

float3 Transmittance(float3 posW, float3 normal, float2 uv) 
{
	// See: http://www.iryoku.com/translucency/
	// See: http://www.iryoku.com/sssss/


	float4 var = Thickness.Sample(LinearSampler, uv);
	float3 t = (float3)0;
	
	[unroll]
	for (int k = 0; k < nrOfShadowLights; k++)
	{
		// Put shrinked vertex in light space
		float4 pos = mul(float4(float3(posW - (0.4 * normal)), 1.0f), translucentData[k].viewProjection);
		pos.xy /= pos.w;

		//Get data from shadowmap
		float d1 = translucentData[k].range * ShadowMaps[k].SampleLevel(LinearSampler, pos.xy, 0).r; // 'd1' has a range of 0..1

		float d = abs(d1 - pos.z);
		float dd = -d * d;
		[loop]
		for (int i = 0; i < nrOfMaterialLayers; i++)
		{
			t += (MaterialLayers[i].xyz * (var.xyz)) * exp(-dd * dd / (MaterialLayers[i].w * (var.w * 255)));
		}
	}
	return	t;
}

geomPixOut main(pixIn inData)
{
	geomPixOut data;

	data.normal = float4(inData.normal, 1.0f);

	data.diff = Diffuse.Sample(LinearSampler, inData.uv);
	
	if(sssEnabled == 0)		data.translucency = float4(Transmittance(inData.posW, inData.normal, inData.uv), 1.0f);
	else					data.translucency = (float4)0;
	//data.translucency = Thickness.Sample(LinearSampler, inData.uv);
	//data.translucency = float4(1, 1, 1, 1);
	data.position = float4(inData.posW, 1.0f);
	
	return data;
}