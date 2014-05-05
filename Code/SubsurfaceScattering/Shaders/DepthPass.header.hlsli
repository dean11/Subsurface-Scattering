
struct vertexIn
{
	float3 posW		: POSITION0;
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;
	//float3 binormal	: BINORMAL0;
	//float3 tangent	: TANGENT0;
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

cbuffer pointLightData :register(b2)
{
	float4 posRange;
};

