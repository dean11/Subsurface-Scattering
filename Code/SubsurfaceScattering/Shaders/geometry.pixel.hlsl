
#include "ShaderData.header.hlsl"

//GEOMETRY PIXEL SHADER

pixOut main(pixIn inData)
{
	pixOut data;

		data.normal = float4(inData.normal, 0.3f);
		data.diff = float4(0.4f, 0.4f, 0.4f, 1.0f);
		data.tranclucent = float4(0.4f, 0.6f, 0.6f, 1.0f);

	return data;
}