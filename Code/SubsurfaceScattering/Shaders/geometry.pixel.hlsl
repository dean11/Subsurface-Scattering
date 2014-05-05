
#include "ShaderData.header.hlsl"

//GEOMETRY PIXEL SHADER

geomPixOut main(pixIn inData)
{
	geomPixOut data;

	data.normal = float4(inData.normal, 1.0f);
	data.diff = Diffuse.Sample(LinearSampler, inData.uv) + CubeMap.Sample(LinearSampler, inData.posW);

	//BasicDirectionalLight(data.diff.xyz, inData.normal, data.diff);

	return data;
}