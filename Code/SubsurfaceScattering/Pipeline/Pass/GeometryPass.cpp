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
		this->GBufferRTV[i]->Release();
	}
	for (int i = 0; i < GBuffer_RTV_Layout_COUNT; i++)
	{
		this->GBufferSRV[i]->Release();
	}
	this->vertex.Release();
	this->pixel.Release();
	this->depthStencilView->Release();
	this->depthStencilUAV->Release();
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

	ID3D11Texture2D* tex1;
	ID3D11Texture2D* tex2;

	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
	texDesc.Width = (UINT)width;
	texDesc.Height = (UINT)height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	//texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	
	if( FAILED( device->CreateTexture2D(&texDesc, 0, &tex1)) )
		return false;
	if (FAILED(device->CreateTexture2D(&texDesc, 0, &tex2)))
		return false;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	{
		rtvDesc.Format = texDesc.Format;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		if( FAILED( device->CreateRenderTargetView(tex1, &rtvDesc, &this->GBufferRTV[GBuffer_RTV_Layout_COLOR])))
			return false;

		if( FAILED( device->CreateRenderTargetView(tex2, &rtvDesc, &this->GBufferRTV[GBuffer_RTV_Layout_NORMAL])))
			return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	{
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		if( FAILED (device->CreateShaderResourceView(tex1, &srvDesc, &this->GBufferSRV[GBuffer_RTV_Layout_COLOR]) ) )
			return false;

		if( FAILED (device->CreateShaderResourceView(tex2, &srvDesc, &this->GBufferSRV[GBuffer_RTV_Layout_NORMAL]) ) )
			return false;
	}

	tex1->Release();
	tex2->Release();

	if (!this->CreateDepthStencil(width, height))
		return false;

	ShaderStates::DepthStencilState::GetDisabledDepth(device);
	ShaderStates::RasterizerState::GetNoCullNoMs(device);
	ShaderStates::SamplerState::GetLinear(device);

	return true;
}
ID3D11ShaderResourceView* GeometryPass::GetShaderResource(GBuffer_RTV_Layout srv)
{
	return this->GBufferSRV[srv];
}
ID3D11RenderTargetView* GeometryPass::GetRenderTarget(GBuffer_RTV_Layout rtv)
{
	return this->GBufferRTV[rtv];
}
void GeometryPass::Apply()
{
	float c[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	for (size_t i = 0; i < GBuffer_RTV_Layout_COUNT; i++)
		this->deviceContext->ClearRenderTargetView(this->GBufferRTV[i], c);

	this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->IASetInputLayout(InputLayoutManager::GetLayout_V_VN_VT());
	this->deviceContext->OMSetRenderTargets(2, this->GBufferRTV, this->depthStencilView);

	this->deviceContext->RSSetState(0);
	this->deviceContext->OMSetDepthStencilState(0, 0);
	this->deviceContext->RSSetState(0);

	this->vertex.Apply();
	this->pixel.Apply();
}

GeometryPass::GeometryPass()
{
	memset(&this->GBufferRTV[0], 0, sizeof(ID3D11RenderTargetView*) * GBuffer_RTV_Layout_COUNT);
	memset(&this->GBufferSRV[0], 0, sizeof(ID3D11ShaderResourceView*) * GBuffer_RTV_Layout_COUNT);
}
GeometryPass::~GeometryPass()
{

}

bool GeometryPass::CreateDepthStencil(int width, int height)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = (UINT)width;
	desc.Height = (UINT)height;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* depthstencil;

	if (FAILED(this->device->CreateTexture2D(&desc, 0, &depthstencil)))
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = 0;
	dsvDesc.Texture2D.MipSlice = 0;
	if (FAILED (this->device->CreateDepthStencilView(depthstencil, &dsvDesc, &this->depthStencilView) ))
	{
		depthstencil->Release();
		return false;
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	if (FAILED (this->device->CreateShaderResourceView(depthstencil, &srvDesc, &this->depthStencilUAV)))
	{
		depthstencil->Release();
		return false;
	}

	depthstencil->Release();
	return true;
}

