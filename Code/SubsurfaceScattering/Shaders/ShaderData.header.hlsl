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
	
	float pad[3];
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
	float cMatrixBuffer_pad[2];
};


Texture2D Diffuse : register(t0);
Texture2D Thickness : register(t1);
TextureCube CubeMap : register(t2);
Texture2D								ShadowMaps[MAX_SHADOWMAPS]		: register(t4);
StructuredBuffer<TranslucentData>		translucentData					: register(t12);
StructuredBuffer<float4>				MaterialLayers					: register(t20);

SamplerState LinearSampler : register(s0);

