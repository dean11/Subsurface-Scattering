


struct vertexIn
{
	float3 posW		: POSITION0;
	float3 normal	: NORMAL0;
	float2 uv		: TEXCOORD0;
	//float3 binormal	: BINORMAL0;
	//float3 tangent	: TANGENT0;
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
	float4 normal		: SV_TARGET0;	//xyz = normal, w = ?
	float4 diff			: SV_TARGET1;	//xyz = diffuse, w = ?
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
TextureCube CubeMap : register(t2);

SamplerState LinearSampler : register(s0);

void BasicDirectionalLight(float3 pixColour, float3 normal, out float4 diffuse)
{
	float3 lightDir = float3(0.0f, 0.0f, 1.0f);
	float4 ambient = float4(0.2f, 0.1f, 0.1f, 1.0f);
	float diffFac = dot(lightDir, normal);
	diffuse = float4(pixColour, 1.0f) * ambient;
	[flatten]
	if (diffFac > 0.0f)
	{
		diffuse += float4((diffFac * pixColour), 1.0f);
	}


}