
#include "ShaderData.header.hlsl"

//GEOMETRY PIXEL SHADER

geomPixOut main(pixIn inData)
{
	geomPixOut data;

	//data.normal = float4(inData.normal, inData.posH.z);
	data.normal = float4(inData.normal, inData.posW.z);
	
	data.diff = Diffuse.Sample(LinearSampler, inData.uv);

	data.position = float4(inData.posW, 1.0f);
	//data.position = float4(0, 0, 0, 1);

	return data;
}