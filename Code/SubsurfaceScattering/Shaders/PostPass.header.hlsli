
#include "Constants.hlsli"



//Some light constant buffers
cbuffer cBuffer :register(b0)
{
	float4 ambientLight;
	float3 cameraPosition;

	int nrOfPointLights;
	int nrOfSpotLights;
	int nrOfDirLights;
	int nrOfShadowLights;

	int sssEnabled;
	//float pad[1];
};

// Shader resource views
Texture2D								DiffuseMap			: register(t0);
Texture2D								NormalMap			: register(t1);
Texture2D								PositionMap			: register(t2);
Texture2D								ThicknessMap		: register(t3);
Texture2D								ShadowMaps[MAX_SHADOWMAPS]		: register(t4);

//Light buffers
StructuredBuffer<PointLight>			pointLights			: register(t9);
StructuredBuffer<SpotLight>				spotLights			: register(t10);
StructuredBuffer<DirLight>				dirLights			: register(t11);
StructuredBuffer<ShadowMapLightData>	shadowLights		: register(t12);

//Output texture..
RWTexture2D<float4> BackBuffer								: register(u0);

//Samplers
SamplerComparisonState ShadowSampler						: register(s0);
SamplerState LinearSampler									: register(s1);

//Methods
float4 PointLightCalc(in PointLight pl, in float3 vertPos, in float3 vNormal)
{
	float3 lightVec = pl.positionRange.xyz - vertPos;
	float dist = length(lightVec);
	float lRange = pl.positionRange.w;
	float att = 1.0f;
	float3 finalCol = (float3)0;

	//In range?
	if (dist <= lRange)
	{
		lightVec /= dist;

		float lightAmount = max(dot(lightVec, vNormal), 0.0f);
	
		att = max(0, 1.0f - (dist / lRange));
		finalCol += att * lightAmount *  pl.diffuse;
	}
	return float4(finalCol, 0.0f);
}
float4 SpotLightCalc(in SpotLight sl, in float3 vertPos, in float3 vNormal)
{
	
	float3 lightVec = sl.pos - vertPos;
	float dist = length(lightVec);
	float att = 1.0f;
	float3 finalCol = (float3)0;
	
	//In range?
	[flatten]
	if (dist <= sl.range)
	{
		lightVec /= dist;
	
		float lightAmount = max(dot(lightVec, vNormal), 0.0f);
		[flatten]
		if (lightAmount > 0.0f)
		{
			finalCol = sl.color / ((sl.att[0] + (sl.att[1] * dist)) + (sl.att[2] * (dist*dist)));
			finalCol *= pow(max(dot(-lightVec, sl.dir), 0.0f), sl.coneAngle);
		}
	}
	return float4(finalCol, 0.0f);
}
float4 DirLightCalc(in DirLight pl, in float3 vertPos, in float3 vNormal)
{
	float diffuseFactor = dot(-pl.direction, vNormal);
	float4 diffuse = (float4)0;

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		diffuse += saturate(float4((diffuseFactor * pl.color), 1.0f)) * pl.intensity;
	}

	return diffuse;
}
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
float ShadowPCF(in float3 posW, in int i, const float4x4 lightViewProj, in const float2 shadowMapSize) 
{
	float litPrecentage = 0.0f;
	
	float4 shadowPosH = mul(float4(posW, 1.0f), lightViewProj);
	shadowPosH.xy /= shadowPosH.w;			// Project the texture coords and scale/offset to [0, 1].
	shadowPosH.z -= 0.001f;				// pixel depth for shadowing is linear.
	
	// Texel size.
	float dx = 2.0f / shadowMapSize.x;
	
	float2 smTex = float2(0.5f * shadowPosH.x, -0.5f * shadowPosH.y) + 0.5f;	//Compute shadow map tex coord
	
	float2 off[9] = 
	{
		float2(-dx, -dx ),	float2(0.0f, -dx ),		float2(+dx, -dx),
		float2(-dx, 0.0f),	float2(0.0f, 0.0f),		float2(+dx, 0.0f),
		float2(-dx, +dx  ),	float2(0.0f, +dx  ),	float2(+dx, +dx),
	};
	
	[unroll]
	for(int i = 0; i < 9; i++)
	{
		litPrecentage += ShadowMaps[i].SampleCmpLevelZero(ShadowSampler, smTex + off[i], shadowPosH.z).r;
	}
	
	return litPrecentage /= 9;
}


float3 T(float s, float4 trans) 
{
	//  http://www.iryoku.com/translucency/
	return	((trans) * exp(-s * s / (trans.w * 255))).xyz;
}

//SSS
half3 SSSTranslucency(	uint i,
						float2 shadowMapSize,
						float4 translucency,
						float3 normalW,
						float3 lightVecW,
						float3 sposW,
						float4x4 lightViewProjection,
						ShadowMapLightData L)
{
	// Put shrinked vertex in light space
	float4 pos = mul(float4(sposW, 1.0f), L.viewProjection);
	pos.xy /= pos.w;

	// Get data from shadowmap
	//float d1 = L.range * ShadowMaps[i].SampleLevel(LinearSampler, pos.xy, 0).r; // 'd1' has a range of 0..1
	float d1 = L.range * ShadowMaps[i][pos.xy * shadowMapSize].r; // 'd1' has a range of 0..1
	float d = abs(d1 - pos.z);
	float dd = -d * d;
	//float3 profile = saturate(T(dd, translucency));
	float3 profile = saturate(translucency.xyz * (translucency.w / d));
	/** 
	* Using the profile, we finally approximate the transmitted lighting from
	* the back of the object:
	*/
	return profile * saturate(dot(lightVecW, -normalW));
}