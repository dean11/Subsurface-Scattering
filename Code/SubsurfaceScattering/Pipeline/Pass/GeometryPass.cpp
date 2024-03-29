#include "GeometryPass.h"

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
	this->inputLayout->Release();
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
		if (!this->vertex.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\basic.vertex.hlsl", "vs_5_0", flag, 0, ShaderType_VS, device, deviceContext))
			return false;

		if (!this->pixel.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\basic.pixel.hlsl", "ps_5_0", flag, 0, ShaderType_PS, device, deviceContext))
			return false;
	}
	else
	{
		if (!this->vertex.LoadCompiledShader("Shaders\\basic.vertex.cso", ShaderType_VS, device, deviceContext))
			return false;

		if (!this->pixel.LoadCompiledShader("Shaders\\basic.pixel.cso", ShaderType_PS, device, deviceContext))
			return false;
	}

	ID3D11Texture2D* tex1;
	ID3D11Texture2D* tex2;
	ID3D11Texture2D* tex3;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = (UINT)width;
	texDesc.Height = (UINT)height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	
	if( FAILED( device->CreateTexture2D(&texDesc, 0, &tex1)) )
		return false;
	if (FAILED(device->CreateTexture2D(&texDesc, 0, &tex2)))
		return false;
	if (FAILED(device->CreateTexture2D(&texDesc, 0, &tex3)))
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

		if( FAILED( device->CreateRenderTargetView(tex3, &rtvDesc, &this->GBufferRTV[GBuffer_RTV_Layout_POSITION])))
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

		if( FAILED (device->CreateShaderResourceView(tex3, &srvDesc, &this->GBufferSRV[GBuffer_RTV_Layout_POSITION]) ) )
			return false;
	}

	tex1->Release();
	tex2->Release();
	tex3->Release();

	if (!this->CreateDepthStencil(width, height))
		return false;

	if (CreateInputLayout())
		return false;

	CreateViewport(width, height);

	return true;
}
void GeometryPass::Apply()
{
	this->deviceContext->RSSetViewports(1, &this->viewPort);

	this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	this->deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->IASetInputLayout(this->inputLayout);

	this->deviceContext->OMSetRenderTargets(GBuffer_RTV_Layout_COUNT, &this->GBufferRTV[0], this->depthStencilView);

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

bool GeometryPass::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	HRESULT hr = S_OK;
	if (FAILED(hr = this->device->CreateInputLayout(desc, 5, this->vertex.GetByteCode(), this->vertex.GetByteCodeSize(), &this->inputLayout)))
		return false;

	this->vertex.RemoveByteCode();

	return true;
}
bool GeometryPass::CreateDepthStencil(int width, int height)
{
	D3D11_TEXTURE2D_DESC desc;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.Height = (UINT)height;
	desc.Width = (UINT)width;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

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
	if (this->device->CreateDepthStencilView(depthstencil, &dsvDesc, &this->depthStencilView) != S_OK)
	{
		depthstencil->Release();
		return false;
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	if (FAILED(this->device->CreateShaderResourceView(depthstencil, &srvDesc, &this->depthStencilUAV)))
	{
		depthstencil->Release();
		return false;
	}

	depthstencil->Release();
	return true;
}
void GeometryPass::CreateViewport(int width, int height)
{
	this->viewPort.TopLeftX = 0;
	this->viewPort.TopLeftY = 0;
	this->viewPort.Width = (FLOAT)width;
	this->viewPort.Height = (FLOAT)height;
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;
}