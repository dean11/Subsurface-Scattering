#include "ShadowVolPass.h"
#include <d3dcompiler.h>
#include "..\InputLayoutState.h"
#include "..\RenderState\DepthStencilState.h"
#include "..\RenderState\RasterizerState.h"
#include "..\RenderState\SamplerState.h"


using namespace Pipeline;
ShadowVolPass::ShadowVolPass()
{

}

ShadowVolPass::~ShadowVolPass()
{

}

void ShadowVolPass::Apply()
{

}

void ShadowVolPass::Release()
{
	for (int i = 0; i < ShadowVol_RTV_Layout_COUNT; i++)
	{
		this->ShadVolRTVs[i].Release();
	}

	this->capsVertex.Release();
	this->capsGeometry.Release();
	this->capsPixel.Release();

	this->volVertex.Release();
	this->volGeometry.Release();
	this->volPixel.Release();
}

bool ShadowVolPass::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile)
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
		if (!this->capsVertex.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\ShadowVolLines.vertex.hlsl", "vs_5_0", flag, 0, ShaderType_VS, device, deviceContext))
			return false;

		if (!this->capsGeometry.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\ShadowVolLines.geometry.hlsl", "gs_5_0", flag, 0, ShaderType_GS, device, deviceContext))
			return false;

		if (!this->capsPixel.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\ShadowVolLines.pixel.hlsl", "ps_5_0", flag, 0, ShaderType_PS, device, deviceContext))
			return false;


		if (!this->volVertex.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\ShadowVol.vertex.hlsl", "vs_5_0", flag, 0, ShaderType_VS, device, deviceContext))
			return false;

		if (!this->volGeometry.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\ShadowVol.geometry.hlsl", "gs_5_0", flag, 0, ShaderType_GS, device, deviceContext))
			return false;

		if (!this->volPixel.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\ShadowVol.pixel.hlsl", "ps_5_0", flag, 0, ShaderType_PS, device, deviceContext))
			return false;

	}
	else
	{
		if (!this->capsVertex.LoadCompiledShader("Shaders\\ShadowVolLines.vertex.cso", ShaderType_VS, device, deviceContext))
			return false;

		if (!this->capsGeometry.LoadCompiledShader("Shaders\\ShadowVolLines.geometry.cso", ShaderType_GS, device, deviceContext))
			return false;

		if (!this->capsPixel.LoadCompiledShader("Shaders\\ShadowVolLines.pixel.cso", ShaderType_PS, device, deviceContext))
			return false;


		if (!this->volVertex.LoadCompiledShader("Shaders\\ShadowVol.vertex.cso", ShaderType_VS, device, deviceContext))
			return false;

		if (!this->volGeometry.LoadCompiledShader("Shaders\\ShadowVol.geometry.cso", ShaderType_GS, device, deviceContext))
			return false;

		if (!this->volPixel.LoadCompiledShader("Shaders\\ShadowVol.pixel.cso", ShaderType_PS, device, deviceContext))
			return false;
	}

	InputLayoutManager::MicrosoftFailedWithDirectX(device, this->capsVertex.GetByteCode(), this->capsVertex.GetByteCodeSize());
	this->capsVertex.RemoveByteCode();

	InputLayoutManager::MicrosoftFailedWithDirectX(device, this->volVertex.GetByteCode(), this->volVertex.GetByteCodeSize());
	this->volVertex.RemoveByteCode();

	if (!CreateDepthStencilAndRenderTargets(width, height)) return false;

	if (!ShaderStates::DepthStencilState::GetEnabledDepth(device)) return false;
	if (!ShaderStates::RasterizerState::GetNoCullNoMs(device)) return false;
	if (!ShaderStates::SamplerState::GetLinear(device)) return false;


	return true;
}

ID3D11ShaderResourceView* ShadowVolPass::GetShaderResource(ShadowVol_RTV_Layout srv)
{
	return this->ShadVolRTVs[srv].GetRenderTargetSRV();
}

ID3D11RenderTargetView* ShadowVolPass::GetRenderTarget(ShadowVol_RTV_Layout rtv)
{
	return this->ShadVolRTVs[rtv].GetRenderTargetView();
}

//Private functions
bool ShadowVolPass::CreateDepthStencilAndRenderTargets(int width, int height)
{
	RenderSurface::TEXTURESURFACE_DESC surfaceDesc;
	surfaceDesc.width = width;
	surfaceDesc.height = height;
	surfaceDesc.depthDesc.dsvTexSubData = 0;
	surfaceDesc.colorDesc.texSubData = 0;

#pragma region DepthStencil
	{
		surfaceDesc.useDepth = true;
		surfaceDesc.useColor = false;

		D3D11_TEXTURE2D_DESC dsvTexDesc;
		dsvTexDesc.Width = (UINT)width;
		dsvTexDesc.Height = (UINT)height;
		dsvTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		dsvTexDesc.Usage = D3D11_USAGE_DEFAULT;
		dsvTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		dsvTexDesc.MipLevels = 1;
		dsvTexDesc.ArraySize = 1;
		dsvTexDesc.SampleDesc.Count = 1;
		dsvTexDesc.SampleDesc.Quality = 0;
		dsvTexDesc.CPUAccessFlags = 0;
		dsvTexDesc.MiscFlags = 0;
		surfaceDesc.depthDesc.dsvTexDesc = &dsvTexDesc;


		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0;
		surfaceDesc.depthDesc.dsvDesc = &dsvDesc;

		D3D11_SHADER_RESOURCE_VIEW_DESC dsvSrvDesc;
		dsvSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		dsvSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsvSrvDesc.Texture2D.MipLevels = 1;
		dsvSrvDesc.Texture2D.MostDetailedMip = 0;
		surfaceDesc.depthDesc.dsvSrvDesc = &dsvSrvDesc;

		if (!this->depthStencil.Create(surfaceDesc, this->device))
			return false;
	}
#pragma endregion

#pragma region RenderTargets
	{
		surfaceDesc.useDepth = false;
		surfaceDesc.useColor = true;

		D3D11_TEXTURE2D_DESC rtvTexDesc;
		memset(&rtvTexDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
		rtvTexDesc.Width = (UINT)width;
		rtvTexDesc.Height = (UINT)height;
		rtvTexDesc.MipLevels = 1;
		rtvTexDesc.ArraySize = 1;
		rtvTexDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		rtvTexDesc.SampleDesc.Count = 1;
		rtvTexDesc.SampleDesc.Quality = 0;
		rtvTexDesc.Usage = D3D11_USAGE_DEFAULT;
		rtvTexDesc.CPUAccessFlags = 0;
		rtvTexDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		rtvTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		surfaceDesc.colorDesc.texDesc = &rtvTexDesc;

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = rtvTexDesc.Format;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		surfaceDesc.colorDesc.rtvDesc = &rtvDesc;

		D3D11_SHADER_RESOURCE_VIEW_DESC rtvSrvDesc;
		rtvSrvDesc.Texture2D.MipLevels = 1;
		rtvSrvDesc.Texture2D.MostDetailedMip = 0;
		rtvSrvDesc.Format = rtvTexDesc.Format;
		rtvSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		surfaceDesc.colorDesc.srvDesc = &rtvSrvDesc;

		if (!this->ShadVolRTVs[ShadowVol_RTV_Layout_VOLUMEFRONT].Create(surfaceDesc, this->device))
			return false;
		if (!this->ShadVolRTVs[ShadowVol_RTV_Layout_VOLUMEBACK].Create(surfaceDesc, this->device))
			return false;
		if (!this->ShadVolRTVs[ShadowVol_RTV_Layout_FRONT].Create(surfaceDesc, this->device))
			return false;
		if (!this->ShadVolRTVs[ShadowVol_RTV_Layout_BACK].Create(surfaceDesc, this->device))
			return false;

	}
#pragma endregion

	return true;
}