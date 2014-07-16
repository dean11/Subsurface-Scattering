
struct vertexIn
{
	float3 posW		: POSITION0;
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;
};

cbuffer perObject	:register(b0)
{
	float4x4 world;
};

cbuffer cMatrixBuffer :register(b1)
{
	float4x4 lightView;
	float4x4 lightProjection;
};

RWTexture2DArray<float4> DepthMap:register(u0);