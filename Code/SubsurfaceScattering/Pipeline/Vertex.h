#ifndef BATCHELOR_VERTEX_H
#define BATCHELOR_VERTEX_H

#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 norm;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT3 tang;
	DirectX::XMFLOAT3 bitang;
};


#endif // !BATCHELOR_VERTEX_H