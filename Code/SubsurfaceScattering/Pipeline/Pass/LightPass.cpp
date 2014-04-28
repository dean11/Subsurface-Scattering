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

void LightPass::Apply()
{}

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
	D3D11_MAPPED_SUBRESOURCE mappedData;
	if (FAILED(this->deviceContext->Map(this->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData))) return;

	memcpy(mappedData.pData, data, sizeof(BasicLightData::PointLight)*count);
	this->deviceContext->Unmap(this->lightBuffer, 0);
	this->pointLight.Apply();
	this->deviceContext->Dispatch(32, 32, 1);
}

void LightPass::RenderSpotLight(const BasicLightData::Spotlight* data, int count)
{
	//TBD
}

void LightPass::RenderDirectionalLight(const BasicLightData::Directional* data, int count)
{
	//TBD
}


//Private functions############################
bool LightPass::CreateSRVAndBuffer(int width, int height)
{
	RenderSurface::TEXTURESURFACE_DESC surfaceDesc;
	surfaceDesc.width = width;
	surfaceDesc.height = height;
	surfaceDesc.depthDesc.dsvTexSubData = 0;
	surfaceDesc.colorDesc.texSubData = 0;

	//#pragma region RenderTargets
	//	{
	//		surfaceDesc.useDepth = false;
	//		surfaceDesc.useColor = true;
	//
	//		D3D11_TEXTURE2D_DESC rtvTexDesc;
	//		memset(&rtvTexDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	//		rtvTexDesc.Width = (UINT)width;
	//		rtvTexDesc.Height = (UINT)height;
	//		rtvTexDesc.MipLevels = 1;
	//		rtvTexDesc.ArraySize = 1;
	//		rtvTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	//		rtvTexDesc.SampleDesc.Count = 1;
	//		rtvTexDesc.SampleDesc.Quality = 0;
	//		rtvTexDesc.Usage = D3D11_USAGE_DEFAULT;
	//		rtvTexDesc.CPUAccessFlags = 0;
	//		rtvTexDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	//		rtvTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//		surfaceDesc.colorDesc.texDesc = &rtvTexDesc;
	//
	//		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	//		rtvDesc.Format = rtvTexDesc.Format;
	//		rtvDesc.Texture2D.MipSlice = 0;
	//		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//		surfaceDesc.colorDesc.rtvDesc = &rtvDesc;
	//
	//		D3D11_SHADER_RESOURCE_VIEW_DESC rtvSrvDesc;
	//		rtvSrvDesc.Texture2D.MipLevels = 1;
	//		rtvSrvDesc.Texture2D.MostDetailedMip = 0;
	//		rtvSrvDesc.Format = rtvTexDesc.Format;
	//		rtvSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//		surfaceDesc.colorDesc.srvDesc = &rtvSrvDesc;
	//
	//		/*if (!this->GBufferRTVs[BPL_RTV_Layout_LIGHTMAP].Create(surfaceDesc, this->device))
	//			return false;*/
	//		//if (!this->GBufferRTVs[BPL_RTV_Layout_DIFFUSE].Create(surfaceDesc, this->device))
	//		//	return false;
	//		//if (!this->GBufferRTVs[BPL_RTV_Layout_SPECULAR].Create(surfaceDesc, this->device))
	//		//	return false;
	//	}
	//#pragma endregion

#pragma region Buffer
	{
		D3D11_BUFFER_DESC bDesc;
		bDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bDesc.ByteWidth = sizeof(BasicLightData::Spotlight) * 512;
		bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bDesc.StructureByteStride = sizeof(BasicLightData::Spotlight);
		bDesc.Usage = D3D11_USAGE_DYNAMIC;

		if (FAILED(this->device->CreateBuffer(&bDesc, 0, &this->lightBuffer))) return false;
	}
#pragma endregion
	return true;
}
