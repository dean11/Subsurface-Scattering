#include "Plane.h"
#include "..\Pipeline\Vertex.h"
#include "..\Pipeline\PipelineManager.h"

Plane::Plane()
{

}
Plane::~Plane()
{

}


bool Plane::CreatePlane(ID3D11Device* device, DirectX::XMFLOAT3 position, const wchar_t texture[], float width, float depth, float scale)
{
	float hw = width * 0.5f;
	float hd = depth * 0.5f;
	Vertex vert[6] =
	{
		{ //[0]
			DirectX::XMFLOAT3(-hw, +0.0f, hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.0f, +0.5f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		},
		{ //[1]
			DirectX::XMFLOAT3(+hw, +0.0f, hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.5f, +0.0f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		},
		{ //[2]
			DirectX::XMFLOAT3(-hw, 0.0f, -hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.0f, +0.5f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		},
		{ //[3]
			DirectX::XMFLOAT3(+hw, 0.0f, +hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.5f, +0.0f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		},
		{ //[4]
			DirectX::XMFLOAT3(+hw, 0.0f, -hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.5f, +0.5f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		},
		{ //[5]
			DirectX::XMFLOAT3(-hw, 0.0f, -hd),		//COORD
			DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),			//Normal
			DirectX::XMFLOAT2(+0.0f, +06.5f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		}
	};

	this->mesh.vertexCount = 6;
	this->mesh.vertexStride = sizeof(Vertex);
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
bool Plane::CreateModel(const char path[], ID3D11Device* device)
{
	return false;
}
void Plane::Render(ID3D11DeviceContext* dc)
{
	UINT off = 0;
	dc->PSSetShaderResources(0, 1, &this->mesh.diffuse);
	dc->IASetVertexBuffers(0, 1, &this->mesh.vertexBuffer, &this->mesh.vertexStride, &off);
	Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(this->world, this->world);
	dc->Draw(this->mesh.vertexCount, 0);
}

