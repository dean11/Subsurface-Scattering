
#include "ShaderData.header.hlsl"

//GEOMETRY PIXEL SHADER

geomPixOut main(pixIn inData)
{
	geomPixOut data;

	data.normal = float4(1.0, 0.0, 0.0, 1.0f);
	data.diff = float4(0.0f, 0.0f, 1.0f, 1.0f);

	return data;
}