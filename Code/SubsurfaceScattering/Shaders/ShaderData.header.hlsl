

struct pixIn
{
	float4 posH			: SV_POSITION;
	float3 posW			: POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXCOORD0;
};

struct pixOut
{
	float4 normal		: SV_TARGET0;	//xyz = normal, w = ?
	float4 diff			: SV_TARGET1;	//xyz = diffuse, w = ?
	float4 tranclucent	: SV_TARGET2;	//xyz = translucent, w = ?
};
