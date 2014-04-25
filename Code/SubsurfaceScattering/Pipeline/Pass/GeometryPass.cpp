#include "GeometryPass.h"
#include "..\InputLayoutState.h"
#include "..\RenderState\DepthStencilState.h"
#include "..\RenderState\RasterizerState.h"
#include "..\RenderState\SamplerState.h"


ID3D11Device *ShaderPass::device = 0;
ID3D11DeviceContext *ShaderPass::deviceContext = 0;

using namespace Pipeline;

#include <d3dcompiler.h>

void GeometryPass::Release()
{
	for (int i = 0; i < GBuffer_RTV_Layout_COUNT; i++)
	{
		this->GBufferRTVs[i].Release();
	}
	this->depthStencil.Release();
	this->vertex.Release();
	this->pixel.Release();
}
bool GeometryPass::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool foreShaderCompile)
{
	this->device = device;
	this->deviceContext = deviceContext;

	UINT flag = 0;
	#if defined(DEBUG) || defined(_DEBUG)
		flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif
	bool result = true;
	std::string file;
	
	if (foreShaderCompile)
	{
		if (!this->vertex.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\geometry.vertex.hlsl", "vs_5_0", flag, 0, ShaderType_VS, device, deviceContext))
			return false;

		if (!this->pixel.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\geometry.pixel.hlsl", "ps_5_0", flag, 0, ShaderType_PS, device, deviceContext))
			return false;
	}
	else
	{
		if (!this->vertex.LoadCompiledShader("Shaders\\geometry.vertex.cso", ShaderType_VS, device, deviceContext))
			return false;

		if (!this->pixel.LoadCompiledShader("Shaders\\geometry.pixel.cso", ShaderType_PS, device, deviceContext))
			return false;
	}

	InputLayoutManager::MicrosoftFailedWithDirectX(device, this->vertex.GetByteCode(), this->vertex.GetByteCodeSize());
	this->vertex.RemoveByteCode();

	if (!CreateDepthStencilAndRenderTargets(width, height)) return false;
	if (!ShaderStates::DepthStencilState::GetDisabledDepth(device)) return false;
	if (!ShaderStates::RasterizerState::GetNoCullNoMs(device)) return false;
	if (!ShaderStates::SamplerState::GetLinear(device)) return false;

	return true;
}
ID3D11ShaderResourceView* GeometryPass::GetShaderResource(GBuffer_RTV_Layout srv)
{
	return this->GBufferRTVs[srv].GetRenderTargetSRV();
}
ID3D11RenderTargetView* GeometryPass::GetRenderTarget(GBuffer_RTV_Layout rtv)
{
	return this->GBufferRTVs[rtv];
}
void GeometryPass::Apply()
{
	float c[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for (size_t i = 0; i < GBuffer_RTV_Layout_COUNT; i++)
		this->deviceContext->ClearRenderTargetView(this->GBufferRTVs[i], c);

	this->deviceContext->ClearDepthStencilView(this->depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->IASetInputLayout(InputLayoutManager::GetLayout_V_VN_VT());
	ID3D11RenderTargetView* rtv[] =
	{
		this->GBufferRTVs[GBuffer_RTV_Layout_NORMAL],
		this->GBufferRTVs[GBuffer_RTV_Layout_COLOR],
	};
	this->deviceContext->OMSetRenderTargets(2, rtv, this->depthStencil);

	this->deviceContext->RSSetState(0);
	this->deviceContext->OMSetDepthStencilState(0, 0);
	this->deviceContext->RSSetState(0);

	this->vertex.Apply();
	this->pixel.Apply();
}

GeometryPass::GeometryPass()
{}
GeometryPass::~GeometryPass()
{}

bool GeometryPass::CreateDepthStencilAndRenderTargets(int width, int height)
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

		D3D11_SHADER_RESOURCE_VIEW_DESC dsvSrvDesc;
		dsvSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		dsvSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		dsvSrvDesc.Texture2D.MipLevels = 1;
		dsvSrvDesc.Texture2D.MostDetailedMip = 0;
		surfaceDesc.depthDesc.dsvSrvDesc = &dsvSrvDesc;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0;
		surfaceDesc.depthDesc.dsvDesc = &dsvDesc;

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

		if (!this->GBufferRTVs[GBuffer_RTV_Layout_COLOR].Create(surfaceDesc, this->device))
			return false;
		if (!this->GBufferRTVs[GBuffer_RTV_Layout_NORMAL].Create(surfaceDesc, this->device))
			return false;
	}
#pragma endregion

	return true;
}


