
#include "ShaderData.header.hlsl"

//GEOMETRY PIXEL SHADER
float ShadowDistance(float3 posW, float3 normalW, int i) 
{
	// shrink vertex and transform to light space:
	float4 posL = mul(float4(posW - (0.5 * normalW), 1.0), translucentData[i].viewProjection);

	// Fetch linear depth from the shadow map:
	float d1 = ShadowMaps[i].SampleLevel(LinearSampler, posL.xy / posL.w, 0).r;
	
	//float d1 = ShadowPCF2(posL, i, 6, translucentData[i].viewProjection, float2(1024, 1024), translucentData[i].range);
	//float d1 = ShadowMaps[i].SampleCmpLevelZero(ShadowSampler, posL, (posL.z - 0.5) / translucentData[i].range).r;
	float d2 = posL.z;

	return abs(d1 * translucentData[i].range - d2);
}
float3 Transmittance(float thickness)
{
	thickness = -thickness * thickness;
	float3 t = (float3)0;
	for (int i = 0; i < nrOfMaterialLayers; i++)
	{
		t += MaterialLayers[i].xyz * exp(thickness / MaterialLayers[i].w);
	}
	return t;
}
float4 Translucency(float3 posW, float3 normal, float2 uv, float4 albedo)
{
	float4 trans = float4(0, 0, 0, 1);

	for (int i = 0; i < nrOfShadowLights; i++)
	{
		float thickness = ShadowDistance(posW, normal, i) / sssStrength;
		float3 transmittance = Transmittance(thickness) * albedo.rgb;
		trans.xyz += transmittance;
	}
	//color += transmittance + reflectance;
	return trans;
}
float3 CalculateTranslucency(float3 posW, float3 normal, float2 uv) 
{
	// See: http://www.iryoku.com/translucency/

	float3 t = (float3)0;
	
	[unroll]
	for (int k = 0; k < nrOfShadowLights; k++)
	{
		// Put shrinked vertex in light space
		// float4 pos = mul(float4(posW - (0.5f * normal), 1.0f), translucentData[k].viewProjection);
		// Get data from shadowmap
		// float d1 = ShadowMaps[k].SampleLevel(LinearSampler, pos.xy / pos.w, 0).r; // 'ShadowMaps[i]' has a range of 0..1

		float d = ShadowDistance(posW, normal, k) / sssStrength;
		//d1 *= translucentData[k].range;
		//
		//float d = abs(d1 - pos.z) / sssStrength;
		float dd = -d * d;
		[loop]
		for (int i = 0; i < nrOfMaterialLayers; i++)
		{
			//t += MaterialLayers[i].xyz * exp(dd / MaterialLayers[i].w);
			t += MaterialLayers[i].xyz * exp(dd / MaterialLayers[i].w);
		}
	}

	return	t;
}


geomPixOut main(pixIn inData)
{
	geomPixOut data;

	data.normal = float4(inData.normal, 1.0f);

	data.diff = Diffuse.Sample(LinearSampler, inData.uv);
	
	//if (sssEnabled == 0)		data.translucency = float4(CalculateTranslucency(inData.posW, inData.normal, inData.uv), 1.0f);
	if (sssEnabled == 0)		data.translucency = Translucency(inData.posW, inData.normal, inData.uv, data.diff);
	else						data.translucency = float4(0, 0, 0, 1);
	data.position = float4(inData.posW, 1.0f);
	
	return data;
}