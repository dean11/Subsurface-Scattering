#include "LightPass.header.hlsl"

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{

	if (DTid.x > 399)
		LightMap[DTid.xy] = float4(1.0f, 0.0f, 0.0f, 1.0f);
	else
		LightMap[DTid.xy] = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
}