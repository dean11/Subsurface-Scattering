
#include "ShaderData.header.hlsl"

// cbuffer perObject		:register(b0)
// cbuffer cMatrixBuffer	:register(b1)

//GEOMETRY VERTEX SHADER

gsIn main(gsIn vIn)
{
	gsIn vOut = (gsIn)0;

	vOut.posW = vIn.posW;
	vOut.normal = vIn.normal;
	vOut.uv = vIn.uv;

	return vOut;
}