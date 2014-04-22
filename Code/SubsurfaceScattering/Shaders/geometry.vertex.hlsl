
#include "ShaderData.header.hlsl"

// cbuffer perObject		:register(b0)
// cbuffer cMatrixBuffer	:register(b1)

//GEOMETRY VERTEX SHADER

pixIn main(vertexIn vIn)
{
	pixIn d = (pixIn)0;

	//mul(vsIn.posW, world);
	//d.posH = float4(vIn.posW, 1.0f);
	//d.posH = mul(float4(vIn.posW, 1.0f), world);
	d.posH = mul(float4(vIn.posW, 1.0f), mul(world, mul(view, projection)));
	//d.posH = float4(0.0f, 0.0f, 0.0f, 1.0f);
	d.posW = vIn.posW;
	d.normal = vIn.normal;
	d.uv = vIn.uv;

	return d;
}