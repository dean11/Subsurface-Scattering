
#include "ShaderData.header.hlsl"

// cbuffer perObject		:register(b0)
// cbuffer cMatrixBuffer	:register(b1)

//GEOMETRY VERTEX SHADER

pixIn main(vertexIn vIn)
{
	pixIn d = (pixIn)0;

	d.posH = mul(float4(vIn.posW, 1.0f), mul(world, mul(view, projection)));
	
	d.posW = mul(float4(vIn.posW, 1.0f), world).xyz;
	
	d.normal = mul(float4(vIn.normal, 0.0f), worlInvTrans);

	d.uv = vIn.uv;

	return d;
}