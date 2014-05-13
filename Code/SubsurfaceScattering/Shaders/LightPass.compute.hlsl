#include "LightPass.header.hlsl"

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	float3 lightCol		= ambientLight.xyz;
	float3 normal		= NormalMap[DTid.xy].xyz;
	float3 posW			= PositionMap[DTid.xy].xyz;
	float3 thickness	= ThicknessMap[DTid.xy].xyz;
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
	
			float lightAmount = max(dot(lightVec, normal), 0.0f);
			float spot = dot(shadowLights[i].direction, -lightVec);
			float s = LightTravelDist(uint3(i, DTid.xy), normal, shrinkedVertPos);
			float irradiance = max(/*0.3 + */dot(-normal, lightVec), 0.0);
			float att = ((shadowLights[i].att[0] + (shadowLights[i].att[1] * dist)) + (shadowLights[i].att[2] * (dist*dist)));
			//if (lightAmount > 0.0f)
			//{
			
			// funciton: float ShadowPCF(float3 posW, int shadowIndex) 
			//float shadow = ShadowPCF(posW, i);

			float3 transmittance = (shadowLights[i].color / att) * pow(max(spot, 0.0f), shadowLights[i].cone);// * shadow;// * irradiance * T(s);
			
			lightCol += transmittance;
		

				//finalCol = shadowLights[t].color / att;
				//finalCol *= pow(max(spot, 0.0f), shadowLights[t].cone);
			//}
		}

		lightCol += finalCol;

		//float3 lightVec = shadowLights[t].position - posW;
        //float dist = length(lightVec);
        //lightVec /= dist;
		//
		//float spot = dot(shadowLights[t].direction, -lightVec);
        //
		//[flatten]
		//if (spot > shadowLights[t].falloffStart) 
		//{
		//	{
		//		// Calculate attenuation:
		//		float curve = min(pow(dist / shadowLights[t].farPlane, 6.0), 1.0);
		//		float attenuation = lerp(1.0 / (1.0 + shadowLights[t].attenuation * dist * dist), 0.0, curve);
		//		
		//		float strength = 1.0f;
		//		// Calculate the distance traveled by the light inside of the object:
		//		// ('strength' modulates the strength of the effect; 'normalW' is the vertex normal)
		//		// function: float LightTravelDist(uint3 i, float3 normalW, float4 shrinkPos) 
		//		float s = LightTravelDist(uint3(t, DTid.xy), normal, shrinkedVertPos) / strength;
		//		
		//		// Estimate the irradiance on the back:
		//		// ('lightW' is the regular light vector)
		//		float irradiance = max(0.3 + dot(-normal, lightVec), 0.0);
		//		
		//		// Calculate transmitted light:
		//		// ('attenuation' and 'spot' are the usual spot light factors, as in regular reflected light):
		//		float3 transmittance = T(s) * shadowLights[t].color * attenuation * spot * irradiance * float3(1.0f, 1.0f, 1.0f);
		//		
		//		// Add the contribution of this light:
		//		lightCol += transmittance; // + reflectance;
		//		//lightCol += shadowLights[t].color * attenuation; // + reflectance;
		//	}
		//
		//	//// Calculate attenuation:
		//	//float curve = min(pow(dist / shadowLights[t].farPlane, 6.0), 1.0);
		//	//float attenuation = lerp(1.0 / (1.0 + shadowLights[t].attenuation * dist * dist), 0.0, curve);
		//	////attenuation = 1.0f;
		//	//
		//	//// And the spot light falloff:
		//	//spot = saturate((spot - shadowLights[t].falloffStart) / shadowLights[t].falloffWidth);
		//	//
		//	//float3 f1 = shadowLights[t].color * attenuation * spot;
		//	//float3 f2 = f1;
		//	//
		//	//	// Calculate the diffuse lighting:
		//	//float3 diffuse = saturate(dot(lightVec, normal));
		//	//
		//	//// And also the shadowing:
		//	////float shadow = ShadowPCF(posW, i, 3, 1.0);
		//	//float shadow = 1.0f;
		//	//
        //   	//
		//	//lightCol += shadow * (f2 * diffuse + f1);
		//	//lightCol = diffuse * attenuation;
		//
		//	//float lightAmount = max(dot(lightVec, normal), 0.0f);
		//	//
		//	//if (lightAmount > 0.0f)
		//	//{
		//	//	lightCol = shadowLights[t].color / shadowLights[t].attenuation;
		//	//	lightCol *= pow(max(dot(-lightVec, shadowLights[t].direction), 0.0f), 45);
		//	//}
		//
		//	
		//	
		//	/*
		//	float3 SSSTranslucency() -> parameters ->
		//	- uint3 shadowIndex,
		//	- float translucency,
		//	- float3 posW,
		//	- float3 normalW,
		//	- float3 lightVecW,
		//	- float4 shrinkedVertPos,
		//	- float4x4 lightViewProjection,
		//	- float lightFarPlane) 
		//	*/
		//	//lightCol = shadowLights[t].view._14_24_34;
		//	//lightCol = shadowLights[t].view._14_24_34;
		//	//lightCol = shadowLights[t].view._41_42_43;
		//	//lightCol += SSSTranslucency(uint3(t, DTid.xy), 1.0f, posW, normal, lightVec, shrinkedVertPos, shadowLights[t].viewProjection, shadowLights[t].farPlane);	
		//}
	}

	LightMap[DTid.xy] = float4(lightCol, 1.0f);
}