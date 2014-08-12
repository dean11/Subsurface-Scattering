#include "Model.h"
#include "..\Importing\ObjGeometryImporter.h"
#include "..\Pipeline\PipelineManager.h"
#include "..\Pipeline\Material.h"

Model::Model()
{
	memset(&this->mesh, 0, sizeof(Model::Mesh));
	this->isVisible = true;
}
Model::~Model()
{
	
}

void Model::Release()
{
	Util::SAFE_RELEASE(this->mesh.diffuse);
	Util::SAFE_RELEASE(this->mesh.vertexBuffer);
}
bool Model::CreateModel(const char path[], ID3D11Device* device, const SimpleMath::Vector4 materialLayer[], int layerCount)
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
	
	wchar_t buff[255] = {0};
	_wgetcwd(buff, 255);
	std::wstring mPath = L"Models\\" + Util::StringToWstring(m[0].map_Kd, std::wstring());
	if (FAILED(hr = DirectX::CreateDDSTextureFromFile(device, mPath.c_str(), nullptr, &this->mesh.diffuse)))
		this->mesh.diffuse = NULL;	//No texture found for the model
	
	DirectX::XMStoreFloat4x4(&this->world, DirectX::XMMatrixIdentity());
	
	this->mesh.materialLayerCount = layerCount;
	this->mesh.materialLayers.Resize(layerCount);
	memcpy(&this->mesh.materialLayers[0], materialLayer, sizeof(materialLayer[0]) * layerCount);
	return true;
}

void Model::DrawModel(ID3D11DeviceContext* dc, bool useTexture)
{
	if(!this->isVisible) return;

	UINT off = 0;
	if(useTexture)
	{
		ID3D11ShaderResourceView* srv[] = { this->mesh.diffuse, 0 };
		dc->PSSetShaderResources(0, Util::NumElementsOf(srv), srv);
	}

	dc->IASetVertexBuffers(0, 1, &this->mesh.vertexBuffer, &this->mesh.vertexStride, &off);

	Pipeline::PipelineManager::Instance().SetMeshBuffer(this->world, this->world.Invert().Transpose(), this->mesh.materialLayers.Size() ? &this->mesh.materialLayers[0] : 0, this->mesh.materialLayerCount);

	dc->Draw(this->mesh.vertexCount, 0);
}
Model::Mesh& Model::GetMesh()
{
	return this->mesh;
}
DirectX::XMMATRIX Model::GetWorld()
{
	return this->world;
}
DirectX::XMMATRIX Model::GetWorld() const
{
	return this->world;
}
DirectX::XMMATRIX Model::GetWorldInversTranspose()
{
	return this->world.Invert().Transpose();
}
DirectX::XMMATRIX Model::GetWorldInversTranspose() const
{
	return this->world.Invert().Transpose();
}

void Model::SetWorld(DirectX::XMFLOAT4X4 world)
{
	this->world = DirectX::SimpleMath::Matrix(&world._11);

}
void Model::SetPosition(float x, float y, float z)
{
	this->world._41 = x;
	this->world._42 = y;
	this->world._43 = z;
}
void Model::SetPosition(DirectX::XMFLOAT3& v)
{
	this->world._41 = v.x;
	this->world._42 = v.y;
	this->world._43 = v.z;
}
void Model::Forward(float val)
{
	SimpleMath::Vector3 v(this->world._31, this->world._32, this->world._33);
	SimpleMath::Vector3 p = v * val + GetPosition();
	SetPosition(p);
}
void Model::Up(float val)
{
	SimpleMath::Vector3 v(this->world._21, this->world._22, this->world._23);
	SimpleMath::Vector3 p = (v * val) + GetPosition();
	SetPosition(p);
}
void Model::Right(float val)
{
	SimpleMath::Vector3 v(this->world._11, this->world._12, this->world._13);
	SimpleMath::Vector3 p = v * val + GetPosition();
	SetPosition(p);
}
void Model::Rotate(const SimpleMath::Vector3& angle)
{
	SimpleMath::Matrix m;
	this->world *= m.CreateFromYawPitchRoll(angle.x, angle.y, angle.z);
	//this->world *= m;
}
void Model::SetScale(float x, float y, float z)
{
	this->world._11 = x;	
	this->world._22 = y;	
	this->world._33 = z;	
}
void Model::SetScale(float s)
{
	this->world._11 = s;	
	this->world._22 = s;	
	this->world._33 = s;	
}
SimpleMath::Vector3 Model::GetPosition()
{
	return SimpleMath::Vector3(this->world._41, this->world._42 ,this->world._43);// = z;
}

void Model::SetMaterial(const SimpleMath::Vector4 materialLayer[], int layerCount)
{
	if (layerCount == 0) return;
	this->mesh.materialLayerCount = layerCount;
	this->mesh.materialLayers.Clear();
	this->mesh.materialLayers.Resize(layerCount);
	try
	{
		memcpy(&this->mesh.materialLayers[0], materialLayer, sizeof(SimpleMath::Vector4) * layerCount);
	}
	catch (std::exception e)
	{
		MessageBox(0, L"TEST", L"", 0);
	}
}