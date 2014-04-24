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

	if (!this->pix.LoadCompiledShader("Shaders\\Skymap.pixel.cso", Pipeline::ShaderType_PS, device, dc))
		return false;

	if (!sphereMap.CreateModel("Models\\sphereMap.obj", device))
		return false;

	return true;
}

void SphereMap::Render(float delta, DirectX::XMFLOAT3 camPos)
{
	DirectX::XMVECTOR scale, trans, nullVec = { 0 };
	DirectX::XMMATRIX tempM;
	DirectX::XMFLOAT4X4 m;
	DirectX::XMFLOAT3 scaleVec = DirectX::XMFLOAT3(1000.0f, 1000.0f, 1000.0f);
	trans = DirectX::XMLoadFloat3(&camPos);
	scale = DirectX::XMLoadFloat3(&scaleVec);
	tempM = DirectX::XMMatrixAffineTransformation(scale, nullVec, nullVec, trans);
	DirectX::XMStoreFloat4x4(&m, tempM);
	//DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixIdentity());
	this->dc->PSSetShaderResources(2, 1, &this->sphereMap.GetMesh().diffuse);
	this->dc->IASetVertexBuffers(0, 1, &this->sphereMap.GetMesh().vertexBuffer, &this->sphereMap.GetMesh().vertexStride, &off);
	Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(m, m);
	pix.Apply();
	this->dc->Draw(this->sphereMap.GetMesh().vertexCount, 0);

}
