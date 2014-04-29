#include "LightPass.header.hlsl"

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	float4 lightCol = 0;
	for (int i = 0; i < nrOfPointLights; i++)
	{
		lightCol += PointLightCalc(pointLights[i], DTid.xy);
	}

	LightMap[DTid.xy] = lightCol;
}