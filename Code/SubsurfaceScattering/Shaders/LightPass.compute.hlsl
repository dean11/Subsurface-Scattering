#include "LightPass.header.hlsl"


[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	float3 lightCol		= ambientLight.xyz;
	float3 normal		= NormalMap[DTid.xy].xyz;
	float3 posW			= PositionMap[DTid.xy].xyz;
	float3 translucency	= ThicknessMap[DTid.xy].xyz;
	//float3 color		= ThicknessMap[DTid.xy].xyz;
	/**
	* First we shrink the position inwards the surface to avoid artifacts:
	* (Note that this can be done once for all the lights)
	*/
	//float4 shrinkedVertPos = float4(posW - (thickness.x * normal), 1.0);
	float4 shrinkedVertPos = float4(posW - (0.05 * normal), 1.0);


	for (int i = 0; i < nrOfPointLights; i++)
		lightCol += PointLightCalc(pointLights[i], posW, normal).xyz;

	for (i = 0; i < nrOfDirLights; i++)
		lightCol += DirLightCalc(dirLights[i], posW, normal).xyz;

	for (i = 0; i < nrOfSpotLights; i++)
		lightCol += SpotLightCalc(spotLights[i], posW, normal).xyz;

	//[unroll]
	for (i = 0; i < nrOfShadowLights; i++)
	{
		float3 lightVec = shadowLights[i].position - posW;
		float dist = length(lightVec);
		float att = 1.0f;
		float3 finalCol = (float3)0.0f;
		
		[flatten]
		if (dist <= shadowLights[i].range)
		{
			lightVec /= dist; 
	
			float diffuse = saturate(max(dot(lightVec, normal), 0.0f));
			float att = ((shadowLights[i].att[0] + (shadowLights[i].att[1] * dist)) + (shadowLights[i].att[2] * (dist*dist)));
			float spot = max(dot(shadowLights[i].direction, -lightVec), 0.0f);

			// funciton: float ShadowPCF(float3 posW, int shadowIndex) 
			float shadow = ShadowPCF(posW, i);
			float3 colorIntensity = (shadowLights[i].color / att);
			float spowl = pow(spot, shadowLights[i].cone);
			lightCol += ( colorIntensity * spowl * diffuse) * shadow;

			#ifdef SSS_ENABLE
			if(sssEnabled == 1)
			{
				/*
				float3 SSSTranslucency(	uint3 shadowIndex,
										float translucency,
										float3 posW,
										float3 normalW,
										float3 lightVecW,
										float4 shrinkedVertPos,
										float4x4 lightViewProjection,
										float lightFarPlane)
				*/
				lightCol += colorIntensity * spowl * SSSTranslucency(uint3(i, DTid.xy), translucency, posW, normal, lightVec, shrinkedVertPos, shadowLights[i].viewProjection, 10.f);
			}
			#endif
		}

		lightCol += finalCol;
	}

	LightMap[DTid.xy] = float4(lightCol, 1.0f);
}