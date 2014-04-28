#include "SphereMap.h"


SphereMap::SphereMap()
{
	//this->cubeSRV = NULL;
}

SphereMap::~SphereMap()
{

}

bool SphereMap::CreateSkyBox(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	this->device = device;
	this->dc = dc;
	/*HRESULT hr;
	if (FAILED(hr = DirectX::CreateDDSTextureFromFile(device, L"Models\\bthcolor.dds", nullptr, &this->cubeSRV)))
		return false;*/

	DirectX::XMVECTOR scale, nullVec = { 0 };
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMFLOAT3 scaleVec = DirectX::XMFLOAT3(1000.0f, 1000.0f, 1000.0f);
	
	scale = DirectX::XMLoadFloat3(&scaleVec);
	worldMatrix = DirectX::XMMatrixAffineTransformation(scale, nullVec, nullVec, nullVec);
	DirectX::XMStoreFloat4x4(&world4x4, worldMatrix);


	if (!this->pix.LoadCompiledShader("Shaders\\Skymap.pixel.cso", Pipeline::ShaderType_PS, device, dc))
		return false;

	if (!sphereMap.CreateModel("Models\\sphereMap.obj", device))
		return false;

	return true;
}

void SphereMap::Render(float delta, DirectX::XMFLOAT3 camPos)
{

	this->dc->PSSetShaderResources(2, 1, &this->sphereMap.GetMesh().diffuse);
	this->dc->IASetVertexBuffers(0, 1, &this->sphereMap.GetMesh().vertexBuffer, &this->sphereMap.GetMesh().vertexStride, &off);
	Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(this->world4x4, this->world4x4);
	pix.Apply();
	this->dc->Draw(this->sphereMap.GetMesh().vertexCount, 0);

}