
#include "ShaderData.header.hlsl"

//GEOMETRY PIXEL SHADER

geomPixOut main(pixIn inData)
{
	geomPixOut data;

	data.normal = float4(inData.normal, 1.0f);
	data.diff = Diffuse.Sample(LinearSampler, inData.uv);

	BasicDirectionalLight(data.diff.xyz, inData.normal, data.diff);

	return data;
}