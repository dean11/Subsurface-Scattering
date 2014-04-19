#include "Model.h"
#include "..\Importing\ObjGeometryImporter.h"

Model::Model()
{

}
Model::~Model()
{

}
void Model::Release()
{

}
bool Model::CreateModel(const char path[], ID3D11Device* device)
{
	ObjGeometryImporter importer;
	std::vector<ObjGeometryImporter::Material> m;
	std::vector<float> v;
	importer.LoadGeometry(path, v, this->mesh.vertexCount, this->mesh.vertexStride, m);

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

	return true;
}
Model::Mesh& Model::GetMesh()
{
	return this->mesh;
}

