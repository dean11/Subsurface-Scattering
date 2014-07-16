#include "Blur.header.hlsli"

[numthreads(BLUR_THREAD_COUNT, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex )
{
	int i = 0;
	//float depth = DepthMap[DTid.xy].r;
	//float4 thick = TranslucencyTex[DTid.xy];
	//float distanceToProjectionWindow = 1.0 / tan(0.5 * radians(45));

	blurCache[GTid.x] = SourceTexture[DTid.xy];
	GroupMemoryBarrierWithGroupSync();

	float4 blurCol = (float4)0;

	for (i = -KernelSize; i <= KernelSize; ++i)
	{
		int p = i + KernelSize;
		int k = GTid.x + p;

		blurCol += blurCache[k] * SampleKernel[p];

		//float offset = GTid.x + SampleKernel[i].a;
		//float4 color = SourceTexture[offset];
		//float4 color = blurCache[offset];

		// If the difference in depth is huge, we lerp color back to "colorM":
		//float s = saturate(300.0f * distanceToProjectionWindow * abs(depth - DepthMap[offset]));
		//color.rgb = lerp(color.rgb, SourceTexture[DTid.xy].rgb, s);
		 
		//blurCol.rgb += SampleKernel[i].rgb * color.rgb;
	}

	BlurDest[DTid.xy] = blurCol;
}