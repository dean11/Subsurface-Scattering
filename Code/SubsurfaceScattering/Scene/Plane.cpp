#include "Plane.h"
#include "..\Pipeline\Vertex.h"
#include "..\Pipeline\PipelineManager.h"
#include <D3DTK\SimpleMath.h>

using namespace DirectX;



bool Plane::CreatePlane(ID3D11Device* device, DirectX::XMFLOAT3 position, const wchar_t texture[], float width, float depth, float scale)
{
	float hw = width * 0.5f;
	float hd = depth * 0.5f;
	VertexPNT vert[6] =
	{
		{ //[0]
			DirectX::XMFLOAT3(-hw, +0.0f, hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.0f, +0.5f),				//UV
		},
		{ //[1]
			DirectX::XMFLOAT3(+hw, +0.0f, hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.5f, +0.0f),				//UV
		},
		{ //[2]
			DirectX::XMFLOAT3(-hw, 0.0f, -hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.0f, +0.5f),				//UV
		},
		{ //[3]
			DirectX::XMFLOAT3(+hw, 0.0f, +hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.5f, +0.0f),				//UV
		},
		{ //[4]
			DirectX::XMFLOAT3(+hw, 0.0f, -hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.5f, +0.5f),				//UV
		},
		{ //[5]
			DirectX::XMFLOAT3(-hw, 0.0f, -hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.0f, +06.5f),				//UV
		}
	};

	this->mesh.vertexCount = 6;
	this->mesh.vertexStride = sizeof(VertexPNT);
	this->mesh.diffuse = 0;
	this->mesh.vertexBuffer = 0;

	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = this->mesh.vertexCount * this->mesh.vertexStride;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = this->mesh.vertexStride;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vert[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	HRESULT hr = S_OK;
	if (FAILED(hr = device->CreateBuffer(&desc, &data, &this->mesh.vertexBuffer)))
		return false;

	if (FAILED(DirectX::CreateDDSTextureFromFile(device, texture, nullptr, &this->mesh.diffuse)))
		return false;

	DirectX::XMStoreFloat4x4(&this->world, DirectX::XMMatrixIdentity());

	DirectX::XMVECTOR nullVec = { 0 };
	DirectX::XMFLOAT3 scaleVec = DirectX::XMFLOAT3(scale, scale, scale);

	DirectX::XMStoreFloat4x4(&this->world, DirectX::XMMatrixAffineTransformation(DirectX::XMLoadFloat3(&scaleVec), nullVec, nullVec, DirectX::XMLoadFloat3(&position)));

	return true;
}