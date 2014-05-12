#include "LightPass.h"
#include "..\InputLayoutState.h"
#include "..\RenderState\DepthStencilState.h"
#include "..\RenderState\RasterizerState.h"
#include "..\RenderState\SamplerState.h"
#include <D3DTK\SimpleMath.h>


struct ConstLightBuffer
{
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4 ambientLight;
	int pointLightCount;
	int spotLightCount;
	int dirLightCount;
	float pad[1];
};

using namespace Pipeline;

#include <d3dcompiler.h>

LightPass::LightPass()
{
	this->maxPointLight = 50;
	this->maxDirLight = 5;
	this->maxSpotLight = 100;

	int temp = 0;
	this->firstPointLight	= (temp += 0);
	this->firstSpotLight	= (temp += maxPointLight);
	this->firstDirLight		= (temp += maxSpotLight);
	
	
}

LightPass::~LightPass()
{
	Release();
}

void LightPass::Release()
{

	this->lightShader.Release();

	if (this->constLightBuffer) this->constLightBuffer->Release(); this->constLightBuffer = 0;

	if (lightMapUAV) lightMapUAV->Release(); lightMapUAV = 0;
	if (lightMapSRV) lightMapSRV->Release(); lightMapSRV = 0;

	if (pointLightBufferSRV) pointLightBufferSRV->Release(); pointLightBufferSRV = 0;
	if (spotLightBufferSRV) spotLightBufferSRV->Release(); spotLightBufferSRV = 0;
	if (dirLightBufferSRV) dirLightBufferSRV->Release(); dirLightBufferSRV = 0;

	if (lightBuffer) lightBuffer->Release(); lightBuffer = 0;
}

void LightPass::Apply(const LightData& lights, ID3D11ShaderResourceView* depthMap, ID3D11ShaderResourceView* normalMap, ID3D11ShaderResourceView* positionMap)
{
	this->deviceContext->CSSetUnorderedAccessViews(0, 1, &this->lightMapUAV, 0);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	if (SUCCEEDED(this->deviceContext->Map(this->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData)))
	{
		BasicLightData::PointLight* l = (BasicLightData::PointLight*)mappedData.pData;
		this->RenderPointLight(&l[this->firstPointLight], lights.pointData, lights.pointCount, lights.view);
		this->RenderDirectionalLight((BasicLightData::Directional*)(&l[this->firstDirLight]), lights.dirData, lights.dirCount, lights.view);
		this->RenderSpotLight((BasicLightData::Spotlight*)(&l[this->firstSpotLight]), lights.spotData, lights.spotCount, lights.view);

		this->deviceContext->Unmap(this->lightBuffer, 0);
	}
	this->lightShader.Apply(this->deviceContext);

	//Set the buffer
	if (SUCCEEDED(this->deviceContext->Map(this->constLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData)))
	{
		ConstLightBuffer* b = (ConstLightBuffer*)mappedData.pData;
		b->dirLightCount = lights.dirCount;
		b->pointLightCount = lights.pointCount;
		b->spotLightCount = lights.spotCount;
		b->ambientLight = DirectX::XMFLOAT4(lights.ambientLight.x, lights.ambientLight.y, lights.ambientLight.z, 0.0f);
		b->proj = DirectX::SimpleMath::Matrix(DirectX::XMLoadFloat4x4(&lights.proj)).Transpose();
		b->view = DirectX::SimpleMath::Matrix(DirectX::XMLoadFloat4x4(&lights.view)).Transpose();

		this->deviceContext->Unmap(this->constLightBuffer, 0);
		this->deviceContext->CSSetConstantBuffers(0, 1, &this->constLightBuffer);
	}
	
	ID3D11ShaderResourceView* srv[] =
	{
		depthMap, 
		normalMap, 
		positionMap,
		this->pointLightBufferSRV,
		this->spotLightBufferSRV,
		this->dirLightBufferSRV
	};
	UINT srvCount = 6;
	this->deviceContext->CSSetShaderResources(0, srvCount, srv);

	ID3D11SamplerState* samp[] = { ShaderStates::SamplerState::GetPoint() };
	this->deviceContext->CSSetSamplers(0, 1, samp);

	this->deviceContext->Dispatch((unsigned int)((this->width + 31) / 32), (unsigned int)((this->height + 31) / 32), 1);
}

bool LightPass::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->width = (unsigned int)width;
	this->height = (unsigned int)height;

	ShaderStates::SamplerState::GetPoint(device);

	UINT flag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	bool result = true;
	std::string file;

	if (forceShaderCompile)
	{

		if (!this->lightShader.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\LightPass.compute.hlsl", "ps_5_0", flag, 0, ShaderType_CS, device, deviceContext))
			return false;
	}
	else
	{

		if (!this->lightShader.LoadCompiledShader("Shaders\\LightPass.compute.cso", ShaderType_CS, device, deviceContext))
			return false;
	}

	if (!CreateSRVAndBuffer(width, height)) return false;
	if (!ShaderStates::DepthStencilState::GetDisabledDepth(device)) return false;
	if (!ShaderStates::RasterizerState::GetNoCullNoMs(device)) return false;
	if (!ShaderStates::SamplerState::GetLinear(device)) return false;

	return true;

}

void LightPass::RenderPointLight(BasicLightData::PointLight* dest, const BasicLightData::PointLight* data, int count, const DirectX::XMFLOAT4X4& view)
{
	if (!count || !data) return;
	int tot = min(this->maxPointLight, count);

	for (int i = 0; i < tot; i++)
	{
		DirectX::XMStoreFloat4(&dest[i].positionRange, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat4(&data[i].positionRange), DirectX::XMLoadFloat4x4(&view)));
		dest[i].positionRange.w = data[i].positionRange.w;

		dest[i].positionRange	= data[i].positionRange;
		dest[i].lightColour		= data[i].lightColour;
	}
}

void LightPass::RenderSpotLight(BasicLightData::Spotlight* dest, const BasicLightData::Spotlight* data, int count, const DirectX::XMFLOAT4X4& view)
{
	if (!count || !data) return;
	int tot = min(this->maxPointLight, count);

	memcpy(dest, data, sizeof(BasicLightData::Spotlight) * tot);
}

void LightPass::RenderDirectionalLight(BasicLightData::Directional* dest, const BasicLightData::Directional* data, int count, const DirectX::XMFLOAT4X4& view)
{
	if (!count || !data) return;
	int tot = min(this->maxDirLight, count);

	memcpy(dest, data, sizeof(BasicLightData::Directional) * tot);
}

ID3D11ShaderResourceView* LightPass::GetLightMapSRV()
{
	return this->lightMapSRV;
}

void LightPass::Clear()
{
	ID3D11ShaderResourceView* nullSRV[] = { 0, 0, 0, 0, 0 };
	this->deviceContext->CSSetShaderResources(0, 4, nullSRV);
	ID3D11UnorderedAccessView* nullUAV[] = { 0 };
	this->deviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
}


// Private functions ############################
bool LightPass::CreateSRVAndBuffer(int width, int height)
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bDesc;
	bDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bDesc.ByteWidth = (sizeof(BasicLightData::Spotlight) * (this->maxDirLight + this->maxPointLight + this->maxSpotLight));
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bDesc.StructureByteStride = sizeof(BasicLightData::Spotlight);
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(this->device->CreateBuffer(&bDesc, 0, &this->lightBuffer))) 
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC sbsrv;
	sbsrv.Format = DXGI_FORMAT_UNKNOWN;
	sbsrv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

	sbsrv.Buffer.ElementOffset = this->firstPointLight;
	sbsrv.Buffer.ElementWidth = this->maxPointLight;
	if (FAILED(this->device->CreateShaderResourceView(this->lightBuffer, &sbsrv, &this->pointLightBufferSRV)))
		return false;	

	sbsrv.Buffer.ElementOffset = this->firstSpotLight;
	sbsrv.Buffer.ElementWidth = this->maxSpotLight;
	if (FAILED(this->device->CreateShaderResourceView(this->lightBuffer, &sbsrv, &this->spotLightBufferSRV)))
		return false;

	sbsrv.Buffer.ElementOffset = this->firstDirLight;
	sbsrv.Buffer.ElementWidth = this->maxDirLight;
	if (FAILED(this->device->CreateShaderResourceView(this->lightBuffer, &sbsrv, &this->dirLightBufferSRV)))
		return false;

	//Create constant buffer
	D3D11_BUFFER_DESC lBdesc;
	lBdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lBdesc.ByteWidth = sizeof(ConstLightBuffer);
	lBdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lBdesc.MiscFlags = 0;
	lBdesc.StructureByteStride = sizeof(ConstLightBuffer);
	lBdesc.Usage = D3D11_USAGE_DYNAMIC;
	if (FAILED(this->device->CreateBuffer(&lBdesc, 0, &this->constLightBuffer)))
		return false;


#pragma region SRV
	{
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.Height = (UINT)height;
		texDesc.Width = (UINT)width;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;

		ID3D11Texture2D* t2D;
		hr = this->device->CreateTexture2D(&texDesc, 0, &t2D);
		if (FAILED(hr)) return false;

		hr = this->device->CreateUnorderedAccessView(t2D, 0, &this->lightMapUAV);
		if (FAILED(hr)) return false;

		hr = this->device->CreateShaderResourceView(t2D, 0, &this->lightMapSRV);
		if (FAILED(hr)) return false;
	}
#pragma endregion
	return true;
}
