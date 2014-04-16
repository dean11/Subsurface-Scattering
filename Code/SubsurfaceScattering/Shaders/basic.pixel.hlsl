
#include "ShaderData.header.hlsl"



pixOut main(pixIn inData) :SV_TARGET3
{
	pixOut data;

		data.normal = float4(1.0f, 1.0f, 1.0f, 1.0f);
		data.diff = float4(1.0f, 1.0f, 1.0f, 1.0f);
		data.tranclucent = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return data;
}