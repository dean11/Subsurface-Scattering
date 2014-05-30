
#include "ShaderData.header.hlsl"

// cbuffer perObject		:register(b0)
// cbuffer cMatrixBuffer	:register(b1)

//GEOMETRY VERTEX SHADER

// Write to a texture in vertex shader? 
//RWTexture2D<float4> BackBuffer								: register(u0);

pixIn main(vertexIn vIn)
{
	pixIn d = (pixIn)0;

	d.posH = mul(float4(vIn.posW, 1.0f), mul(world, mul(view, projection)));
	
	d.posW = mul(float4(vIn.posW, 1.0f), world).xyz;
	
	//d.normal = mul(float4(vIn.normal, 1.0f), worlInvTrans);
	d.normal = vIn.normal;

	//BackBuffer[d.posH.xy] = (float4)1.0f;
	d.uv = vIn.uv;

	return d;
}