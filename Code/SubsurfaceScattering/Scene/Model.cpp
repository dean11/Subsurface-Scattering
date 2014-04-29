#include "Model.h"
#include "..\Importing\ObjGeometryImporter.h"

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
	std::vector<float> v;
	if (!importer.LoadGeometry(path, v, this->mesh.vertexCount, this->mesh.vertexStride, m))
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
	return this->world;
}
DirectX::XMFLOAT4X4 Model::GetWorldInversTranspose() const
{
	return this->world;
}

void Model::SetWorld(DirectX::XMFLOAT4X4 world)
{
	this->world = world;
}




