#include "LightPass.header.hlsl"

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	float3 lightCol = ambientLight.xyz;

	float3 normal = NormalMap[DTid.xy].xyz;
	float3 posW = PositionMap[DTid.xy].xyz;

	for (int p = 0; p < nrOfPointLights; p++)
	{
		lightCol += PointLightCalc(pointLights[p], posW, normal).xyz;
	}

	for (int d = 0; d < nrOfDirLights; d++)
	{
		lightCol += DirLightCalc(dirLights[d], posW, normal).xyz;
	}
	
	for (int s = 0; s < nrOfSpotLights; s++)
	{
		lightCol += SpotLightCalc(spotLights[s], posW, normal).xyz;
	}

	
	LightMap[DTid.xy] = float4(lightCol, 1.0f);

}