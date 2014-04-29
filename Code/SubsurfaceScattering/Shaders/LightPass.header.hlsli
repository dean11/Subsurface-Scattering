struct DirLight
{
	float4 diffuse;
};

struct SpotLight
{
	float4 diffuse;

};

struct PointLight
{
	float4 diffuse;
	float4 positionRange;
	float4 attenuation;
};

cbuffer perObject	:register(b0)
{
	float4x4 world;
	float4x4 worlInvTrans;
};

cbuffer cMatrixBuffer :register(b1)
{
	float4x4 view;
	float4x4 projection;
};


Texture2D Diffuse : register(t0);
TextureCube CubeMap : register(t1);

StructuredBuffer<PointLight> pointLights : register(t2);
StructuredBuffer<SpotLight> spotLights : register(t3);
StructuredBuffer<DirLight> dirLights : register(t4);

RWTexture2D<float4> LightMap : register(u0);

SamplerState LinearSampler : register(s0);
