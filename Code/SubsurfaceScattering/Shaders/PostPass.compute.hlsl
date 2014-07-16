#include "PostPass.header.hlsli"


[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	//float3 finalColor		= ambientLight.xyz;
	
	float3 normal			= NormalMap[DTid.xy].xyz;
	float3 albedo			= DiffuseMap[DTid.xy].xyz;
	float3 posW				= PositionMap[DTid.xy].xyz;
	float4 translucency		= ThicknessMap[DTid.xy];
	float3 finalColor		= ambientLight.xyz * albedo;
	
	// Shrink the position inwards the surface to avoid artifacts in sss pass:
	float3 shrinkedVertPos = float3(posW - (0.4 * normal));

	[loop]
	for (int i = 0; i < nrOfPointLights; i++)
		finalColor += albedo * PointLightCalc(pointLights[i], posW, normal).xyz;

	[loop] 
	for (i = 0; i < nrOfDirLights; i++)
		finalColor += albedo * DirLightCalc(dirLights[i], posW, normal).xyz;

	[loop]
	for (i = 0; i < nrOfSpotLights; i++)
		finalColor += albedo * SpotLightCalc(spotLights[i], posW, normal).xyz;
	
	[unroll]
	for (i = 0; i < nrOfShadowLights; i++)
	{
		ShadowMapLightData L = shadowLights[i];
		float2 shadowMapSize = ShadowMaps[i].Length.xy;

		float3 lightVec = L.position - posW;
		float d = length(lightVec);
		
		if (d <= L.range)
		{
			float3 D = 0.0f;

			lightVec /= d; 
			
			float diffuse = saturate(dot(lightVec, normal));
			float3 f1 = (float3)0.0f;
			
			//float spot = pow(abs(dot(-lightVec, L.direction)), L.spot);
			float spot = pow(max((dot(-lightVec, L.direction)), 0), L.spot);
			float att = spot / dot(L.att, float3(1.0f, d, d*d));
			f1 = (albedo * L.color * att * spot );
			// float shadow = ShadowPCF2(posW, i, 3, L.viewProjection, shadowMapSize, L.range);
			D = f1  * diffuse;// *shadow;
			
			#ifdef SSS_ENABLE
				if(sssEnabled == 0)
				{
					D += f1 * (translucency * saturate(dot(lightVec, -normal)));
					// SSSTranslucency(uint i,float2 shadowMapSize,float4 translucency,float3 normalW,float3 lightVecW,float3 sposW,float4x4 lightViewProjection,ShadowMapLightData L)
					//D += f1 * SSSTranslucency(i, shadowMapSize, translucency, normal, lightVec, shrinkedVertPos, L.viewProjection, L);
				}
			#endif

			finalColor += D;
		}
	}
	
	BackBuffer[DTid.xy] = float4(finalColor, 1.0f);
	//BackBuffer[DTid.xy] = translucency;
}