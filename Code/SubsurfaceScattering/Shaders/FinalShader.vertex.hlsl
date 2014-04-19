#include "ShaderData.header.hlsl"

vOutFSQ main(vertexIn dt)
{
	vOutFSQ o;
	o.posH = float4(dt.posW, 1.0);
	o.uv = dt.uv;
	return o;
}