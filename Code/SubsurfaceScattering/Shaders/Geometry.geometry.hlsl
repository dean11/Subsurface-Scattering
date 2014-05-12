#include "ShaderData.header.hlsl"

[maxvertexcount(12)]
void main(triangleadj gsIn vIn[6], inout TriangleStream< pixIn > output)
{
	pixIn d = (pixIn)0;
	for (uint i = 0; i < 6; i+=2)
	{
		d.posH = mul(float4(vIn[i].posW, 1.0f), mul(world, mul(view, projection)));

		d.posW = mul(float4(vIn[i].posW, 1.0f), world).xyz;
		//d.posW = mul(float4(vIn.posW, 1.0f), mul(world, view)).xyz;

		//d.normal = mul(float4(vIn.normal, 0.0f), worlInvTrans);
		d.normal = vIn[i].normal;

		d.uv = vIn[i].uv;
		
		output.Append(d);
	}

	output.RestartStrip();
}