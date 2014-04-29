struct DirLight
{
	float4 diffuse;
	float4 pad0;
	float4 pad1;
	float4 pad2;
};

struct SpotLight
{
	float4 diffuse;
	float4 pad0;
	float4 pad1;
	float4 pad2;
};

struct PointLight
{
	float4 diffuse;
	float4 positionRange;
	float4 attenuation;
	float4 pad;
};

cbuffer cLightBuffer :register(b0)
{
	float4x4 invProj;
	int nrOfPointLights;
	int nrOfSpotLights;
	int nrOfDirLights;
	float pad;
};

Texture2D DepthMap : register(t0);
Texture2D NormalMap : register(t1);

StructuredBuffer<PointLight> pointLights : register(t2);
StructuredBuffer<SpotLight> spotLights : register(t3);
StructuredBuffer<DirLight> dirLights : register(t4);

RWTexture2D<float4> LightMap : register(u0);

SamplerState LinearSampler : register(s0);


float4 PointLightCalc(in PointLight pl, float3 posWV, in uint2 patch)
{
	float4 normal = NormalMap[patch];
		float3 lPos = pl.positionRange.xyz;
		float radius = pl.positionRange.w;
	float3 lVec = lPos - posWV;
		float lightRange = length(lVec);
	if (lightRange > radius)
	{

	}
	return float4(1.0f, 0.0f, 0.0f, 0.0f);
}
float4 SpotLightCalc(in SpotLight pl, float3 posWV, in uint2 patch)
{

	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}
float4 DirLightCalc(in DirLight pl, float3 posWV, in uint2 patch)
{

	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}