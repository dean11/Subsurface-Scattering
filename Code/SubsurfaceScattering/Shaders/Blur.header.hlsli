
#define BLUR_THREAD_COUNT 256

cbuffer BlurData : register(b0)
{
	// Size of blur kernel including first sample.
	int KernelSize;
};

//Albedo texture
Texture2D<float4> SourceTexture :register(t0);

//Thickness texture
Texture2D<float4> TranslucencyTex :register(t1);

//Depth map
Texture2D<float4> DepthMap :register(t2);

//Kernel samples
StructuredBuffer<float4> SampleKernel :register(t3);

//Blur kernel
groupshared float4 blurCache[BLUR_THREAD_COUNT];

// Blur output data
RWTexture2D<float4> BlurDest :register(u0);

//Point sampler state
SamplerState PointSampler :register(s0);
SamplerState LinearSampler :register(s1);
