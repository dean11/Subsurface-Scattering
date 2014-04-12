#include "DeferredPipeline.h"

using namespace Pipeline;

#include <d3dcompiler.h>

static DeferredPipeline* deferredPipeline = 0;


DeferredPipeline& DeferredPipeline::Instance()
{
	if(!deferredPipeline) deferredPipeline = new DeferredPipeline();
	return *deferredPipeline;
}
void DeferredPipeline::Release()
{
	for (int i = 0; i < GBuffer_RTV_Layout_COUNT; i++)
	{
		deferredPipeline->GBufferRTV[i]->Release();
	}
	for (int i = 0; i < GBuffer_RTV_Layout_COUNT; i++)
	{
		deferredPipeline->GBufferSRV[i]->Release();
	}
	deferredPipeline->vertex.Release();
	deferredPipeline->pixel.Release();

	delete deferredPipeline;
	deferredPipeline = 0;

}
bool DeferredPipeline::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	UINT flag = 0;
	#if defined(DEBUG) || defined(_DEBUG)
		flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

	bool forceReload = true;
	std::wstring file;
	
	file = forceReload ? L"..\\GLPathTracing\\Shaders\\basic.vertex.hlsl" : L"Shaders\\basic.vertex.cso";
	if(!this->vertex.CreateShader(file.c_str(), "vs_5_0", flag, 0, ShaderType_VS, device, deviceContext))
		return false;
	
	file = forceReload ? L"..\\GLPathTracing\\Shaders\\basic.pixel.hlsl" : L"Shaders\\basic.pixel.cso";
	if(!this->pixel.CreateShader(file.c_str(), "ps_5_0", flag, 0, ShaderType_PS, device, deviceContext))
		return false;

	ID3D11Texture2D* tex;
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
	
	if( FAILED( device->CreateTexture2D(&texDesc, 0, &tex)) )
		return false;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	{
		rtvDesc.Format = texDesc.Format;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		if( FAILED( device->CreateRenderTargetView(tex, &rtvDesc, &this->GBufferRTV[GBuffer_RTV_Layout_COLOR])))
			return false;

		if( FAILED( device->CreateRenderTargetView(tex, &rtvDesc, &this->GBufferRTV[GBuffer_RTV_Layout_NORMAL])))
			return false;

		if( FAILED( device->CreateRenderTargetView(tex, &rtvDesc, &this->GBufferRTV[GBuffer_RTV_Layout_POSITION])))
			return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	{
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		if( FAILED (device->CreateShaderResourceView(tex, &srvDesc, &this->GBufferSRV[GBuffer_RTV_Layout_COLOR]) ) )
			return false;

		if( FAILED (device->CreateShaderResourceView(tex, &srvDesc, &this->GBufferSRV[GBuffer_RTV_Layout_NORMAL]) ) )
			return false;

		if( FAILED (device->CreateShaderResourceView(tex, &srvDesc, &this->GBufferSRV[GBuffer_RTV_Layout_POSITION]) ) )
			return false;
	}

	tex->Release();

	return true;
}
void DeferredPipeline::Render()
{
	//this->vertex.Apply();
	this->pixel.Apply();



}

DeferredPipeline::DeferredPipeline()
{
	memset(&this->GBufferRTV[0], 0, sizeof(ID3D11RenderTargetView*) * GBuffer_RTV_Layout_COUNT);
	memset(&this->GBufferSRV[0], 0, sizeof(ID3D11ShaderResourceView*) * GBuffer_RTV_Layout_COUNT);
}
DeferredPipeline::~DeferredPipeline()
{

}