#include "PointLight.h"
#include "..\Importing\ObjGeometryImporter.h"

PointLight::PointLight()
{

}

PointLight::~PointLight()
{
	Release();
}

void PointLight::Release()
{
	if (this->pointLight.sphere.GetMesh().vertexBuffer) this->pointLight.sphere.GetMesh().vertexBuffer->Release();
	this->pointLight.sphere.GetMesh().vertexBuffer = NULL;
}

bool PointLight::CreatePointLight(const char path[], BasicLightData::PointLight data, ID3D11Device* device)
{
	if (!this->pointLight.sphere.CreateModel(path, device)) return false;

	this->pointLight.light = data;

	//Calculating world matrix of type XMFLOAT4X4 for the point light
	DirectX::XMFLOAT4X4 world;
	DirectX::XMMATRIX worldMatrix;
	DirectX::XMVECTOR scaleVec, transVec, nullVec = { 0 };
	DirectX::XMFLOAT3 scale, trans;
	scale = DirectX::XMFLOAT3(this->pointLight.light.positionRange.w, this->pointLight.light.positionRange.w, this->pointLight.light.positionRange.w);
	trans = DirectX::XMFLOAT3(this->pointLight.light.positionRange.x, this->pointLight.light.positionRange.y, this->pointLight.light.positionRange.z);

	scaleVec = DirectX::XMLoadFloat3(&scale);
	transVec = DirectX::XMLoadFloat3(&trans);
	worldMatrix = DirectX::XMMatrixAffineTransformation(scaleVec, nullVec, nullVec, transVec);
	DirectX::XMStoreFloat4x4(&world, worldMatrix);

	this->pointLight.sphere.SetWorld(world);

	return true;
}

PointLight::Light& PointLight::GetPointLight()
{
	return this->pointLight;
}
