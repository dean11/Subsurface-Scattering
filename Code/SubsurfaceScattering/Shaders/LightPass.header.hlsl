
//Standarnd Lights
struct DirLight
{
	float3 color;
	float3 direction;
	int shadowMap;

	float pad[1 + 4 + 4];
};
struct SpotLight
{
	float3 pos;
	float range;
	float3 dir;
	float coneAngle;
	float3 att;
	float3 color;
	int shadowMap;

	float pad[1];
};
struct PointLight
{
	float4 positionRange;
	float3 diffuse;
	int shadowMap;

	float pad[4 + 4];
};

//ShadowMap lights
struct ShadowMapLightData
{
	float4x4 viewProjection;
	float3 position;
	float3 direction;
	float3 att;
	float3 color;
	float range;
	float cone;
	int shadowIndex;
	float pad[1];
};

//Some light constant buffers
cbuffer cLightBuffer :register(b0)
{
	float4x4 invProj;

	float4 ambientLight;
	float3 cameraPosition;

	int nrOfPointLights;
	int nrOfSpotLights;
	int nrOfDirLights;
	int nrOfShadowLights;

	float pad[2];
};

// Deferred textures
Texture2D								NormalMap			: register(t0);
Texture2D								PositionMap			: register(t1);
Texture2D								ThicknessMap		: register(t2);
Texture2D								ShadowMaps[5]		: register(t3);

//Light buffers
StructuredBuffer<PointLight>			pointLights			: register(t8);
StructuredBuffer<SpotLight>				spotLights			: register(t9);
StructuredBuffer<DirLight>				dirLights			: register(t10);
StructuredBuffer<ShadowMapLightData>	shadowLights		: register(t11);

//Output texture..
RWTexture2D<float4> LightMap								: register(u0);

//Samplers
SamplerState PointSampler									: register(s0);
SamplerComparisonState ShadowSampler						: register(s1);

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
	

		if (lightAmount > 0.0f)
		{
			att = max(0, 1.0f - (dist / lRange));
			finalCol += att * lightAmount *  pl.diffuse;
		}
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
		diffuse += saturate(float4((diffuseFactor * pl.color), 1.0f));
	}

	return diffuse;
}
float ShadowPCF(float3 posW, int shadowIndex) 
{
	float2 len = ShadowMaps[shadowIndex].Length.xy;
	float4 shadowPosH = mul(float4(posW, 1.0f), shadowLights[shadowIndex].viewProjection);

	// Project the texture coords and scale/offset to [0, 1].
	shadowPosH.xy /= shadowPosH.w;
	float depth = shadowPosH.z / shadowPosH.w; // pixel depth for shadowing.
	float dx = 1.0f / len.x;
	float2 smTex = float2(0.5f*shadowPosH.x, -0.5f*shadowPosH.y) + 0.5f;	//Compute shadow map tex coord
	
	float2 off[9] = 
	{
		float2(-dx, -dx ),	float2(0.0f, -dx ),	float2(dx, -dx),
		float2(-dx, 0.0f),	float2(0.0f, 0.0f),	float2(dx, 0.0f),
		float2(-dx, dx  ),	float2(0.0f, dx  ),	float2(dx, dx),
	};
	
	float lit = 0.0f;
	for(int i = 0; i < 9; i++)
	{
		lit += ShadowMaps[shadowIndex].SampleCmpLevelZero(ShadowSampler, smTex + off[i], depth).r;
	}

	lit /= 9;
	return lit;

	/*
	float4 shadowPosition = mul(float4(worldPosition, 1.0), shadowLights[i].viewProjection);
    shadowPosition.xy /= shadowPosition.w;
    //shadowPosition.z += shadowLights[i].bias;
    
    float2 size = ShadowMaps[i].Length.xy;

    float shadow = 0.0;
    float offset = (samples - 1.0) / 2.0;
    [unroll]
    for (float x = -offset; x <= offset; x += 1.0) 
	{
        [unroll]
        for (float y = -offset; y <= offset; y += 1.0) 
		{
            float3 pos = float3((shadowPosition.xy + width * float2(x, y) / size.x), i);
           // shadow += ShadowMaps[i].SampleCmpLevelZero(ShadowSampler, pos, (shadowPosition.z / shadowLights[i].farPlane)).r;
        }
    }
    shadow /= samples * samples;
    return shadow;
	*/
}


float LightTravelDist(uint3 i, float3 normalW, float4 shrinkedVertexPosition) 
{
	// Source: http://www.iryoku.com/translucency/

	// Transform to light space:
	float4 posLightSpace = mul(shrinkedVertexPosition, shadowLights[i.x].viewProjection);

	// Fetch depth from the shadow map: (Depth from shadow maps is linear)
	float d1 = ShadowMaps[i.x][ ((posLightSpace.xy / posLightSpace.w)  * (ShadowMaps[i.x].Length.xy))].r;
	float d2 = posLightSpace.z;

	// Calculate the difference:
	return abs(d1 - d2);
}
float3 T(float s) 
{
	// Source: http://www.iryoku.com/translucency/

	return	float3(0.233, 0.455, 0.649) * exp(-s * s / 0.0064) +
			float3(0.1,   0.336, 0.344) * exp(-s * s / 0.0484) +
			float3(0.118, 0.198, 0.0)   * exp(-s * s / 0.187)  +
			float3(0.113, 0.007, 0.007) * exp(-s * s / 0.567)  +
			float3(0.358, 0.004, 0.0)   * exp(-s * s / 1.99)   +
			float3(0.078, 0.0,   0.0)   * exp(-s * s / 7.41);
}

//SSS
float3 SSSTranslucency(	uint3 shadowIndex,
						float translucency,
						float3 posW,
						float3 normalW,
						float3 lightVecW,
						float4 shrinkedVertPos,
						float4x4 lightViewProjection,
						float lightFarPlane)
{
    /**
     *	Now we calculate the thickness from the light point of view:
     */
	float4 shadowPosition = mul(shrinkedVertPos, lightViewProjection);
	//float d1 = SSSSSample(shadowMap, shadowPosition.xy / shadowPosition.w).r; // 'd1' has a range of 0..1
	//float d1 = ShadowMaps[shadowIndex.x][shadowPosition.xy / shadowPosition.w].r; // 'd1' has a range of 0..1
	float d1 = ShadowMaps[shadowIndex.x][((shadowPosition.xy / shadowPosition.w) * (ShadowMaps[shadowIndex.x].Length.xy))].r; // 'd1' has a range of 0..1
	float d2 = shadowPosition.z; // 'd2' has a range of 0..'lightFarPlane'
	d1 *= lightFarPlane; // So we scale 'd1' accordingly:
	float d = abs(d1 - d2);

    /**
     * Armed with the thickness, we can now calculate the color by means of the
     * precalculated transmittance profile.
     * (It can be precomputed into a texture, for maximum performance):
     */
	float dd = -d * d;
	float3 profile = T(dd);

    /** 
     * Using the profile, we finally approximate the transmitted lighting from
     * the back of the object:
     */
    return profile * saturate(0.3 + dot(lightVecW, -normalW));
    //return saturate(0.3 + dot(lightVecW, -normalW));
}

//-----------------------------------------------------------------------------
// Separable SSS Reflectance Vertex Shader