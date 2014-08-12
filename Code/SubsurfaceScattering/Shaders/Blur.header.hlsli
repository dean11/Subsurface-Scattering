
#define THREAD_COUNT	256
#define BLUR_RADIUS		5
#define CACHE_SIZE		(THREAD_COUNT + (2 * BLUR_RADIUS))

cbuffer SSSData : register(b0)
{
	float sssStrength;
	static const int brad = 5;
};

//Albedo texture
Texture2D<float4> SourceTexture :register(t0);

//Thickness texture
Texture2D<float4> LayerTex :register(t1);

//Depth map
Texture2D<float4> DepthMap :register(t2);

//Kernel samples
StructuredBuffer<float4> Kernel :register(t3);
//StructuredBuffer<float> Kernel :register(t3);

//Blur kernel
groupshared float4 blurCache[CACHE_SIZE];
groupshared float4 depthCache[CACHE_SIZE];
// Blur output data
RWTexture2D<float4> BlurDest :register(u0);

//Point sampler state
SamplerState PointSampler :register(s0);
SamplerState LinearSampler :register(s1);


float4 BlurPixel(in float2 dir, in uint2 DTid, in uint2 GTid, in int index)
{
	// See: http://www.iryoku.com/sssss/

	float4 color = blurCache[dot(GTid, dir) + BLUR_RADIUS];
	float depth = DepthMap[DTid.xy].r;
	float4 layer = LayerTex[DTid.xy];
	float4 blurColor = float4(color.rgb * Kernel[0].rgb, color.a);

	//float2 pixelSize = 1.0 / tan(0.5 * radians(45));
	//float2 step = color.a * (sssStrength * (BLUR_RADIUS) * pixelSize * dir) / depth;

	float distanceToProjectionWindow = 1.0 / tan(0.5 * radians(45));
	float scale = distanceToProjectionWindow / depth;
	float2 step = sssStrength * scale * dir;
	step *= color.a;
	step *= 1.0f / 3.0f;
	
	//blurCache[GTid.x + BLUR_RADIUS]
	[unroll]
	for (int i = -BLUR_RADIUS + 1; i <= BLUR_RADIUS; ++i)
	{
		int k = i + BLUR_RADIUS;
		//int2 p = (DTid + ((i + BLUR_RADIUS) * dir)) + Kernel[k].w * step;
		//float3 colorTmp = SourceTexture[p].rgb;
		int p = dot(GTid, dir) + (i + BLUR_RADIUS) + Kernel[k].w * dot(step, dir);
		float3 colorTmp = blurCache[p].rgb;
		float depthTmp = depthCache[p].r;
		//float depthTmp = DepthMap[(DTid + ((i + BLUR_RADIUS) * dir)) + Kernel[k].w * step].r;

		// If the difference in depth is huge lerp color back to "colorM":
		float s = min(distanceToProjectionWindow * sssStrength * abs(depth - depthTmp), 1.0);
		colorTmp = lerp(colorTmp, color.rgb, s);
		
		// Accumulate:
		float3 f = float3(ceil(layer.rgb.x), ceil(layer.rgb.y), ceil(layer.rgb.z));
		blurColor.rgb += (f * Kernel[k].rgb * colorTmp.rgb);
	}

	return blurColor;
}

#ifdef TEST
float4 BlurPS(PassV2P input, uniform float2 step) : SV_TARGET
{
	// Gaussian weights for the six samples around the current pixel:
	//   -3 -2 -1 +1 +2 +3
	float w[6] = { 0.006, 0.061, 0.242, 0.242, 0.061, 0.006 };
	float o[6] = { -1.0, -0.6667, -0.3333, 0.3333, 0.6667, 1.0 };

	// Fetch color and linear depth for current pixel:
	float4 colorM = colorTex.Sample(PointSampler, input.texcoord);
	float depthM = depthTex.Sample(PointSampler, input.texcoord);

	// Accumulate center sample, multiplying it with its gaussian weight:
	float4 colorBlurred = colorM;
		colorBlurred.rgb *= 0.382;

	// Calculate the step that we will use to fetch the surrounding pixels,
	// where "step" is:
	//     step = sssStrength * gaussianWidth * pixelSize * dir
	// The closer the pixel, the stronger the effect needs to be, hence
	// the factor 1.0 / depthM.
	float2 finalStep = colorM.a * step / depthM;

		// Accumulate the other samples:
		[unroll]
	for (int i = 0; i < 6; i++)
	{
		// Fetch color and depth for current sample:
		float2 offset = input.texcoord + o[i] * finalStep;
			float3 color = colorTex.SampleLevel(LinearSampler, offset, 0).rgb;
			float depth = depthTex.SampleLevel(PointSampler, offset, 0);

		// If the difference in depth is huge, we lerp color back to "colorM":
		float s = min(0.0125 * correction * abs(depthM - depth), 1.0);
		color = lerp(color, colorM.rgb, s);

		// Accumulate:
		colorBlurred.rgb += w[i] * color;
	}

	// The result will be alpha blended with current buffer by using specific
	// RGB weights. For more details, I refer you to the GPU Pro chapter :)
	return colorBlurred;
}
#endif