#include "ShaderData.header.hlsl"

//SKYMAP PIXEL SHADER

geomPixOut main(pixIn inData)
{
	geomPixOut data;

	/*data.normal = float4(inData.normal, 1.0f);
	data.diff = CubeMap.Sample(LinearSampler, inData.posW);*/

	//BasicDirectionalLight(data.diff.xyz, inData.normal, data.diff);

	return data;
}