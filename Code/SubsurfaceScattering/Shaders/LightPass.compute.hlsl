#include "LightPass.header.hlsl"


[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	float3 lightCol		= ambientLight.xyz;
	float3 normal		= NormalMap[DTid.xy].xyz;
	float3 posW			= PositionMap[DTid.xy].xyz;
	float4 translucency	= ThicknessMap[DTid.xy];
	//float3 color		= ThicknessMap[DTid.xy].xyz;
	/**
	* First we shrink the position inwards the surface to avoid artifacts:
	* (Note that this can be done once for all the lights)
	*/
	//float4 shrinkedVertPos = float4(posW - (thickness.x * normal), 1.0);
	float3 shrinkedVertPos = float3(posW - (0.05 * normal));

	[loop]
	for (int i = 0; i < nrOfPointLights; i++)
		lightCol += PointLightCalc(pointLights[i], posW, normal).xyz;

	[loop]
	for (i = 0; i < nrOfDirLights; i++)
		lightCol += DirLightCalc(dirLights[i], posW, normal).xyz;

	[loop]
	for (i = 0; i < nrOfSpotLights; i++)
		lightCol += SpotLightCalc(spotLights[i], posW, normal).xyz;
	
	[unroll]
	for (i = 0; i < nrOfShadowLights; i++)
	{
		ShadowMapLightData light = shadowLights[i];
		float3 lightVec = light.position - posW;
		float dist = length(lightVec);
		float2 shadowMapSize = ShadowMaps[i].Length.xy;
		
		//[flatten]
		if (dist <= light.range)
		{
			//Normalize the light vector
			lightVec /= dist; 
			
			float spot1 = dot( light.direction, -lightVec );
			float3 att = ((light.att.x + (light.att.y * dist)) + (light.att.z * (dist*dist)));
			float spot = pow( max(dot(light.direction, -lightVec), 0.0f), light.cone);
			//float spot = ((dot(light.direction, -lightVec) - light.cone) / 0.05f);
			float totalLightCoff = max(dot(lightVec, normal), 0);
			
			//lightCol += light.color / att;
			//lightCol *= (spot * diffuse) * ShadowPCF(posW, i, light.viewProjection, shadowMapSize);
			lightCol = ((lightCol + light.color) / att) * spot * totalLightCoff * ShadowPCF(posW, i, light.viewProjection, shadowMapSize);
			#ifdef SSS_ENABLE
			if(sssEnabled == 1)
			{
				// function : float3 SSSTranslucency(uint i, float2 size, float4 translucency, float3 normalW, float3 lightVecW, float3 shrinkedVertPos, float4x4 lightViewProjection, float lightFarPlane)
				float3 sss = SSSTranslucency(i, shadowMapSize, translucency, normal, lightVec, shrinkedVertPos, light.viewProjection, 10.f);
				lightCol += (spot * sss) / att;
			}
			#endif
		}
	}

	LightMap[DTid.xy] = float4(lightCol, 1.0f);
}