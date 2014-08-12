#include "Blur.header.hlsli"

[numthreads(1, THREAD_COUNT, 1)]
void main( uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex )
{
	if (GTid.y < BLUR_RADIUS)
	{
		int y = max(DTid.y - BLUR_RADIUS, 0);
		blurCache[GTid.y] = SourceTexture[int2(DTid.x, y)];
		depthCache[GTid.y] = DepthMap[int2(DTid.x, y)];
	}
	if (GTid.y >= THREAD_COUNT - BLUR_RADIUS)
	{
		int y = min(DTid.y + BLUR_RADIUS, SourceTexture.Length.y - 1);
		blurCache[GTid.y + (2 * BLUR_RADIUS)] = SourceTexture[int2(DTid.x, y)];
		int dy = min(DTid.y + BLUR_RADIUS, DepthMap.Length.y - 1);
		depthCache[GTid.y + (2 * BLUR_RADIUS)] = DepthMap[int2(DTid.x, y)];
	}

	blurCache[GTid.y + BLUR_RADIUS] = SourceTexture[min(DTid.xy, SourceTexture.Length.xy - 1)];
	depthCache[GTid.y + BLUR_RADIUS] = DepthMap[min(DTid.xy, DepthMap.Length.xy - 1)];

	GroupMemoryBarrierWithGroupSync();

	BlurDest[DTid.xy] = BlurPixel(float2(0, 1), DTid.xy, GTid.xy, GTid.y);

	//float4 blurCol = SourceTexture[min(DTid.xy, SourceTexture.Length.xy - 1)];
	//[unroll]
	//for (i = -BLUR_RADIUS + 1; i <= BLUR_RADIUS; ++i)
	//{
	//	// Fetch color and depth for current sample:
	//	float2 offset = DTid.xy + Kernel[i].a * float2(sssStrength, 0);
	//	float4 color = SourceTexture[offset];
	//
	//	// If the difference in depth is huge, we lerp color back to "colorM":
	//	float depthTmp = DepthMap[offset].r;
	//	float s = saturate(0.5f * sssStrength * abs(depth - depthTmp));
	//	color.rgb = lerp(color.rgb, SourceTexture[DTid.xy].rgb, s);
	//
	//	// Accumulate:
	//	blurCol.rgb += (thick.rgb * Kernel[i].rgb * color.rgb);
	//}
	//
	//BlurDest[DTid.xy] = blurCol;
}


