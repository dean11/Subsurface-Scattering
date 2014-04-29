struct vertexIn
{
	float3 posW		: POSITION0;
	float2 uv		: TEXCOORD0;
};
struct vOutFSQ
{
	float4 posH	: SV_POSITION;
	float2 uv	: TEXCOORD0;
};

Texture2D gSRVNormal			:register(t0);
Texture2D gSRVColor				:register(t1);
Texture2D gLightMap				:register(t2);