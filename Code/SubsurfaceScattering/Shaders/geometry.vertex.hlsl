
#include "ShaderData.header.hlsl"

//GEOMETRY VERTEX SHADER

pixIn main(vertexIn vIn)
{
	pixIn d = (pixIn)0;
	d.posH = float4(0.0f, 0.0f, 0.0f, 1.0f);
	d.posW = vIn.posW;
	d.normal = vIn.normal;
	d.uv = vIn.uv;
	return d;
}