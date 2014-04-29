#include "LightPass.h"
#include "..\InputLayoutState.h"
#include "..\RenderState\DepthStencilState.h"
#include "..\RenderState\RasterizerState.h"
#include "..\RenderState\SamplerState.h"


using namespace Pipeline;

#include <d3dcompiler.h>

LightPass::LightPass()
{

}

LightPass::~LightPass()
{
	Release();
}

void LightPass::Release()
{

	this->pointLight.Release();
}

void LightPass::Apply(ID3D11ShaderResourceView* diffuseMap, ID3D11ShaderResourceView* normalMap)
{
	ID3D11ShaderResourceView* srvMap[] = {diffuseMap, normalMap};
	this->deviceContext->CSSetShaderResources(0, 2, srvMap);
}

bool LightPass::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile)
{
	this->device = device;
	this->deviceContext = deviceContext;

	UINT flag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	bool result = true;
	std::string file;

	if (forceShaderCompile)
	{

		if (!this->pointLight.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\PointLightShader.compute.hlsl", "ps_5_0", flag, 0, ShaderType_CS, device, deviceContext))
			return false;
		/*if (!this->spotLight.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\PointLightShader.compute.hlsl", "ps_5_0", flag, 0, ShaderType_PS, device, deviceContext))
			return false;
		if (!this->dirLight.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\PointLightShader.compute.hlsl", "ps_5_0", flag, 0, ShaderType_PS, device, deviceContext))
			return false;*/
	}
	else
	{

		if (!this->pointLight.LoadCompiledShader("Shaders\\PointLightShader.compute.cso", ShaderType_CS, device, deviceContext))
			return false;
		/*if (!this->spotLight.LoadCompiledShader("Shaders\\PointLightShader.compute.cso", ShaderType_PS, device, deviceContext))
			return false;
		if (!this->dirLight.LoadCompiledShader("Shaders\\PointLightShader.compute.cso", ShaderType_PS, device, deviceContext))
			return false;*/
	}

	if (!CreateSRVAndBuffer(width, height)) return false;
	if (!ShaderStates::DepthStencilState::GetDisabledDepth(device)) return false;
	if (!ShaderStates::RasterizerState::GetNoCullNoMs(device)) return false;
	if (!ShaderStates::SamplerState::GetLinear(device)) return false;

	return true;

}

void LightPass::RenderPointLight(const BasicLightData::PointLight* data, int count)
{
	if (!count || !data) return;

	D3D11_MAPPED_SUBRESOURCE mappedData;
	if ( SUCCEEDED(this->deviceContext->Map(this->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData) ) )
	{
		memcpy(mappedData.pData, data, sizeof(BasicLightData::PointLight)*count);
		this->deviceContext->Unmap(this->lightBuffer, 0);
		
		//Set the buffer
		this->deviceContext->CSSetShaderResources(0, 1, &this->lightBufferSRV);

		this->pointLight.Apply();
		this->deviceContext->Dispatch(32, 32, 1);
	}
}

void LightPass::RenderSpotLight(const BasicLightData::Spotlight* data, int count)
{
	if (!count || !data) return;
	//TBD
}

void LightPass::RenderDirectionalLight(const BasicLightData::Directional* data, int count)
{
	if (!count || !data) return;
	//TBD
}


// Private functions ############################
bool LightPass::CreateSRVAndBuffer(int width, int height)
{
	D3D11_BUFFER_DESC bDesc;
	bDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bDesc.ByteWidth = sizeof(BasicLightData::Spotlight) * 512;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bDesc.StructureByteStride = sizeof(BasicLightData::Spotlight);
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(this->device->CreateBuffer(&bDesc, 0, &this->lightBuffer))) return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC sbsrv;
	sbsrv.Format = DXGI_FORMAT_UNKNOWN;
	sbsrv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	sbsrv.Buffer.ElementOffset = 0;
	sbsrv.Buffer.ElementWidth = sizeof(BasicLightData::Spotlight);
	if (FAILED(this->device->CreateShaderResourceView(this->lightBuffer, &sbsrv, &this->lightBufferSRV)))
		return false;
	
	return true;
}
