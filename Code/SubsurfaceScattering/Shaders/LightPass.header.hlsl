struct DirLight
{
	float3 color;
	float3 direction;
	float pad[1 + 1 + 4 + 4];
};

struct SpotLight
{
	float3 pos;
	float range;
	float3 dir;
	float coneAngle;
	float3 att;
	float3 color;

	float pad[1 + 1];
};

struct PointLight
{
	float4 positionRange;
	float3 diffuse;
	float pad[1 + 4 + 4];
};

struct ShadowMap
{

};

cbuffer cLightBuffer :register(b0)
{
	float4x4 invProj;
	float4 ambientLight;
	int nrOfPointLights;
	int nrOfSpotLights;
	int nrOfDirLights;
	float pad;
};

Texture2D DepthMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D PositionMap : register(t2);

StructuredBuffer<PointLight> pointLights : register(t3);
StructuredBuffer<SpotLight> spotLights : register(t4);
StructuredBuffer<DirLight> dirLights : register(t5);

RWTexture2D<float4> LightMap : register(u0);

SamplerState PointSampler : register(s0);

float4 PointLightCalc(in PointLight pl, in float3 vertPos, in float3 vNormal)
{
	float3 lightVec = pl.positionRange.xyz - vertPos;
	float dist = length(lightVec);
	float lRange = pl.positionRange.w;
	float att = 1.0f;

	//In range?
	if (dist > lRange)
	{
		return (float4)0.0f;
	}
	
	lightVec /= dist;

	float lightAmount = max(dot(lightVec, vNormal), 0.0f);
	float3 finalCol = (float3)0;

	if (lightAmount > 0.0f)
	{
		att = max(0, 1.0f - (dist / lRange));
		finalCol += att * lightAmount *  pl.diffuse;
	}
	return float4(finalCol, 0.0f);
}
float4 SpotLightCalc(in SpotLight sl, in float3 vPos, in float3 vNormal)
{
	
	float3 lightVec = sl.pos - vPos;
	float dist = length(lightVec);
	float att = 1.0f;
	
	//In range?
	if (dist > sl.range)
		return (float4)0.0f;
	
	lightVec /= dist;
	
	float lightAmount = max(dot(lightVec, vNormal), 0.0f);

	float3 finalCol = (float3)0;
	
	if (lightAmount > 0.0f)
	{
		finalCol = sl.color / ((sl.att[0] + (sl.att[1] * dist)) + (sl.att[2] * (dist*dist)));
		finalCol *= pow(max(dot(-lightVec, sl.dir), 0.0f), sl.coneAngle);
	}
	return float4(finalCol, 0.0f);
}
float4 DirLightCalc(in DirLight pl, in float3 pos, in float3 normal)
{
	float diffuseFactor = dot(-pl.direction, normal);
	float4 diffuse = (float4)0;

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		diffuse += saturate(float4((diffuseFactor * pl.color), 1.0f));
	}

	return diffuse;
}