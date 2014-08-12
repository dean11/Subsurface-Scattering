#include "Constants.hlsli"

struct vertexIn
{
	float3 posW		: POSITION0;
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;
};
struct vOutFSQ
{
	float4 posH	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};
struct pixIn
{
	float4 posH			: SV_POSITION;
	float3 posW			: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
};

struct geomPixOut
{
	float4 normal		: SV_TARGET0;	// xyz = normal,	w = ?
	float4 diff			: SV_TARGET1;	// xyz = diffuse,	w = ?
	float4 position		: SV_TARGET2;	// xyz = pos,		w = ?
	float4 translucency	: SV_TARGET3;	// xyz = trans,		w = ?
};
struct TranslucentData
{
	float4x4 viewProjection;
	float range;
	float3 position;
};

cbuffer perObject	:register(b0)
{
	float4x4 world;
	float4x4 worlInvTrans;
	int nrOfMaterialLayers;
	
	float perObjectPad[3];
};

cbuffer cMatrixBuffer :register(b1)
{
	float4x4 view;
	float4x4 projection;
	int nrOfShadowLights;
	int sssEnabled;
	float sssStrength;
	float cMatrixBuffer_pad[1];
};


Texture2D Diffuse : register(t0);
Texture2D PAD0 : register(t1);
TextureCube CubeMap : register(t2);
Texture2D								ShadowMaps[MAX_SHADOWMAPS]		: register(t4);
StructuredBuffer<TranslucentData>		translucentData					: register(t12);
StructuredBuffer<float4>				MaterialLayers					: register(t20);

SamplerState LinearSampler : register(s0);
SamplerState PointSampler	:register(s1);
SamplerComparisonState ShadowSampler :register(s2);

float ShadowPCF2(float3 worldPosition, int i, int samples, float4x4 vp, in const float2 shadowMapSize, float range)
{
	float4 shadowPosition = mul(float4(worldPosition, 1.0), vp);
		shadowPosition.xy /= shadowPosition.w;
	shadowPosition.z -= 0.5f;

	float w = shadowMapSize.x;
	float h = shadowMapSize.y;

	float shadow = 0.0;
	float offset = (samples - 1.0) / 2.0;
	[unroll]
	for (float x = -offset; x <= offset; x += 1.0)
	{
		[unroll]
		for (float y = -offset; y <= offset; y += 1.0)
		{
			float2 pos = shadowPosition.xy + 1.0f * float2(x, y) / w;
				shadow += ShadowMaps[i].SampleCmpLevelZero(ShadowSampler, pos, shadowPosition.z / range).r;
		}
	}
	shadow /= samples * samples;
	return shadow;
}