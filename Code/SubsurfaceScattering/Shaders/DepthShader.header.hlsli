
struct vertexIn
{
	float3 posW		: POSITION0;
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;
};

cbuffer perObject	:register(b0)
{
	float4x4 world;
	float4x4 worlInvTrans;
};

cbuffer cMatrixBuffer :register(b1)
{
	float4x4 lightView;
	float4x4 lightProjection;
};
