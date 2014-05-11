

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


Texture2D gSRVColor				:register(t0);			 //SrvMap_Normal = 1,			//:register(1)
Texture2D gSRVNormal			:register(t1);			 //SrvMap_Diffuse = 0,			//:register(0)
Texture2D gLightMap				:register(t2);			 //SrvMap_Light = 2,			//:register(2)
Texture2D gThicknessMap			:register(t3);			 //SrvMap_Thickness = 3,		//:register(3)

Texture2D gShadowMap			:register(t4);			 //SrvMap_shadowMap = 4,		//:register(4)

//Buffer for lightmatrix for shadow casting
//StructuredBuffer<LightData> LightMatrix	:register(t5);	

RWTexture2D<float4> FinalImage	:register(u0);

SamplerComparisonState ShadowSampler :register(s0)
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
	ComparisonFunc = LESS;
};

float CalcShadow(in float4 shadowPosH)
{
	float2 dim = gShadowMap.Length.xy;

	// Project the texture coords and scale/offset to [0, 1].
	shadowPosH.xy /= shadowPosH.w;
	float depth = shadowPosH.z / shadowPosH.w; // pixel depth for shadowing.
	float dx = 1.0f / dim.x;
	float2 smTex = float2(0.5f*shadowPosH.x, -0.5f*shadowPosH.y) + 0.5f;	//Compute shadow map tex coord

	float2 off[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, dx), float2(0.0f, dx), float2(dx, dx),
	};

	float shadowCoff = 0.0f;
	for (int i = 0; i<9; i++)
	{
		shadowCoff += gShadowMap.SampleCmpLevelZero(ShadowSampler, smTex + off[i], depth).r;
	}

	shadowCoff /= 9;

	return shadowCoff;
}