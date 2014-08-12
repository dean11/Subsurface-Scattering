#include "Blur.header.hlsli"

[numthreads(THREAD_COUNT, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex )
{
	if (GTid.x < BLUR_RADIUS)
	{
		int x = max(DTid.x - BLUR_RADIUS, 0);
		blurCache[GTid.x] = SourceTexture[int2(x, DTid.y)];
		depthCache[GTid.x] = DepthMap[int2(x, DTid.y)];
	}
	if (GTid.x >= THREAD_COUNT - BLUR_RADIUS)
	{
		int x = min(DTid.x + BLUR_RADIUS, SourceTexture.Length.x - 1);
		blurCache[GTid.x + (2 * BLUR_RADIUS)] = SourceTexture[int2(x, DTid.y)];
		int dx = min(DTid.x + BLUR_RADIUS, DepthMap.Length.x - 1);
		depthCache[GTid.x + (2 * BLUR_RADIUS)] = DepthMap[int2(x, DTid.y)];
	}

	blurCache[GTid.x + BLUR_RADIUS] = SourceTexture[min(DTid.xy, SourceTexture.Length.xy - 1)];
	depthCache[GTid.x + BLUR_RADIUS] = DepthMap[min(DTid.xy, DepthMap.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	BlurDest[DTid.xy] = BlurPixel(float2(1, 0), DTid.xy, GTid.xy, GTid.x);
}


