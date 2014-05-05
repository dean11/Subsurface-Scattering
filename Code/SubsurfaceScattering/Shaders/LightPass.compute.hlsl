#include "LightPass.header.hlsl"

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	float4 lightCol = (float4)0;

	//Calculate world*view pos
	float2 uv = DTid.xy / NormalMap.Length.xy;
	uv.x = uv.x * 2 - 1;
	uv.y = 1 - 2 * uv.y;

	// Get screen space and depth value, unproject the screen cordinates.
	float4 viewPos = mul(invProj, float4(DTid.x, DTid.y, DepthMap[DTid.xy].x, 1.0f));
	float3 pos = viewPos.xyz / viewPos.w;

	[loop]
	for (int i = 0; i < nrOfPointLights; i++)
	{
		lightCol += PointLightCalc(pointLights[i], pos,  DTid.xy);

	}
	[loop]
	for (i = 0; i < nrOfSpotLights; i++)
	{
		
	}
	[loop]
	for (i = 0; i < nrOfDirLights; i++)
	{
	
	}

	LightMap[DTid.xy] = float4(pos, 1.0);
}