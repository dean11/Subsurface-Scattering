#include "Model.h"
#include "..\Importing\ObjGeometryImporter.h"
#include <D3DX10mesh.h>
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3d10.lib")



Model::Model()
{
	memset(&this->mesh, 0, sizeof(Model::Mesh));
}
Model::~Model()
{

}
void Model::Release()
{
	if (this->mesh.vertexBuffer) this->mesh.vertexBuffer->Release(); this->mesh.vertexBuffer = 0;
}
bool Model::CreateModel(const char path[], ID3D11Device* device)
{
	ObjGeometryImporter importer;
	std::vector<ObjGeometryImporter::Material> m;
	std::vector<ObjGeometryImporter::VertexVNT> v;
	std::vector<unsigned int> index;
	if (!importer.LoadGeometry(path, v, index, this->mesh.indexCount, this->mesh.vertexCount, this->mesh.vertexStride, m))
		return false;
	
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = this->mesh.vertexCount * this->mesh.vertexStride;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = this->mesh.vertexStride;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &v[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	HRESULT hr = S_OK;
	if (FAILED(hr = device->CreateBuffer(&desc, &data, &this->mesh.vertexBuffer)))
		return false;

	CreateAdjData(v, index);
	this->mesh.indexCount = index.size();
	D3D11_BUFFER_DESC indexDesc;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(unsigned int) * (this->mesh.indexCount);
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;
	indexDesc.StructureByteStride = 0;
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = &index[0];
	idata.SysMemPitch = 0;
	idata.SysMemSlicePitch = 0;
	//this->mesh.indexCount = (unsigned int)index.size();
	if (FAILED(hr = device->CreateBuffer(&indexDesc, &idata, &this->mesh.indexBuffer)))
		return false;

	

	
	std::wstring mPath = L"Models\\" + Util::StringToWstring(m[0].map_Kd, std::wstring());
	if (FAILED(DirectX::CreateDDSTextureFromFile(device, mPath.c_str(), nullptr, &this->mesh.diffuse)))
		this->mesh.diffuse = NULL;	//No texture found for the model

	DirectX::XMStoreFloat4x4(&this->world, DirectX::XMMatrixIdentity());
	
	return true;
}
Model::Mesh& Model::GetMesh()
{
	return this->mesh;
}
DirectX::XMFLOAT4X4 Model::GetWorld()
{
	return this->world;
}
DirectX::XMFLOAT4X4 Model::GetWorld() const
{
	return this->world;
}
DirectX::XMFLOAT4X4 Model::GetWorldInversTranspose()
{
	return this->world.Invert().Transpose();
}
DirectX::XMFLOAT4X4 Model::GetWorldInversTranspose() const
{
	return this->world.Invert().Transpose();
}

void Model::SetWorld(DirectX::XMFLOAT4X4 world)
{
	this->world = DirectX::SimpleMath::Matrix(&world._11);

}

void Model::CreateAdjData(std::vector<ObjGeometryImporter::VertexVNT> &v, std::vector<unsigned int> &index)
{
	std::vector<unsigned int> indices(v.size());
	for (size_t i = 0; i < v.size(); i++)
		indices[i] = i;// index[i];
	ID3DX10Mesh *mesh = 0;
	ID3D10Device *dummy = 0;
	const void* vData = &v[0];
	const void* iData = &indices[0];
	const unsigned int numVerts = v.size();
	HRESULT hr;
	if (SUCCEEDED(hr = D3D10CreateDevice(NULL, D3D10_DRIVER_TYPE_REFERENCE, 0, 0, D3D10_SDK_VERSION, &dummy)))
	{
		D3D10_INPUT_ELEMENT_DESC desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};
		if (SUCCEEDED(hr = D3DX10CreateMesh(dummy, desc, 3, "POSITION", numVerts, numVerts / 3, D3DX10_MESH_32_BIT, &mesh)))
		{
			if (SUCCEEDED(hr = mesh->SetIndexData(iData, numVerts)))
			{
				ID3DX10MeshBuffer *tempIdx = 0;
				SIZE_T s = 0;
				unsigned int *ix = 0;
				mesh->GetIndexBuffer(&tempIdx);
				int f = tempIdx->GetSize();
				if (SUCCEEDED(hr = mesh->SetVertexData(0, vData)))
				{
					//if (SUCCEEDED(hr = mesh->GenerateAdjacencyAndPointReps(0.0001f)))
					{
						
						if (SUCCEEDED(hr = mesh->GenerateGSAdjacency()))
						{
							ID3DX10MeshBuffer *tempIdx = 0;
							SIZE_T s = 0;
							unsigned int *ix = 0;
							mesh->GetIndexBuffer(&tempIdx);
							int t = tempIdx->GetSize();
							tempIdx->Map((void**)&ix, &s);
							tempIdx->Unmap();

							/*char msg[100];
							sprintf_s(msg, "%d %d %d %d %d %d", ix[0], ix[1], ix[2], ix[3], ix[4], ix[5]);
							MessageBoxA(0, msg, "", 0);*/
							index.clear();
							index.resize(0);
							for (SIZE_T i = 0; i < s; i++)
								index.push_back(ix[i]);

							if (tempIdx)
							{
								tempIdx->Release();
								tempIdx = NULL;
							}
						}
					}
				}
			}
		}
	}
	
	if (dummy)
	{
		dummy->Release();
		dummy = NULL;
	}

	if (mesh)
	{
		mesh->Release();
		mesh = NULL;
	}
}




