#include "FinalPass.h"
#include "..\RenderState\DepthStencilState.h"
#include "..\RenderState\RasterizerState.h"
#include "..\RenderState\SamplerState.h"
#include "..\InputLayoutState.h"
#include "..\Vertex.h"
#include "..\..\Utilities\Util.h"

#include <d3d11_2.h>
#include <d3dcompiler.h>
using namespace Pipeline;

FinalPass::FinalPass()
{

}
FinalPass::~FinalPass()
{

}
void FinalPass::Release()
{
	Util::SAFE_RELEASE(this->backBufferUAV);

	this->compute.Release();
}
void FinalPass::Apply(DispatchTextureData& srvData)
{	
	this->Clear();

	this->compute.Apply();
	ID3D11UnorderedAccessView* uav[] = { this->backBufferUAV };
	this->deviceContext->CSSetUnorderedAccessViews(0, Util::NumElementsOf(uav), uav, 0);
		
	ID3D11ShaderResourceView* srv[SrvMap_COUNT] = { 0 };
	srv[SrvMap_Diffuse]		= srvData.DiffuseMap;
	srv[SrvMap_Light]		= srvData.LightMap;
	srv[SrvMap_Normal]		= srvData.NormalMap;
	srv[SrvMap_shadowMap]	= srvData.ShadowMap;
	srv[SrvMap_Thickness]	= srvData.ThicknessMap;
	this->deviceContext->CSSetShaderResources(0, Util::NumElementsOf(srv), srv);

	this->isCleared = false;
	

	this->deviceContext->Dispatch((unsigned int)((this->width + 31) / 32), (unsigned int)((this->height + 31) / 32), 1);
}

bool FinalPass::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool foreShaderCompile, IDXGISwapChain* swap)
{
	HRESULT hr = S_OK;
	this->deviceContext = deviceContext;
	
	ShaderStates::DepthStencilState::GetDisabledDepth(device);
	ShaderStates::RasterizerState::GetNoCullNoMs(device);
	ShaderStates::SamplerState::GetLinear(device);

	if (!this->compute.LoadCompiledShader("Shaders\\FinalPass.compute.cso", ShaderType_CS, device, deviceContext))
		return false;

	ID3D11Texture2D *bb;
	if (FAILED(swap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&bb))))
	{
		printf("Failed to get BackBuffer from Swapchain");
		return false;
	}

	if (FAILED(hr = device->CreateUnorderedAccessView(bb, 0, &this->backBufferUAV)))
	{
		printf("Failed to create unordered access view with back buffer!");
		return false;
	}

	D3D11_TEXTURE2D_DESC td;
	bb->GetDesc(&td);
	this->width = td.Width;
	this->height = td.Height;

	bb->Release();
	return true;
}
void FinalPass::Clear()
{
	if (!this->isCleared)
	{
		this->deviceContext->CSSetShader(0, 0, 0);
		ID3D11ShaderResourceView* srvClear[SrvMap_COUNT] = { 0 };
		this->deviceContext->CSSetShaderResources(0, SrvMap_COUNT, srvClear);

		ID3D11UnorderedAccessView* uav[] = { 0 };
		this->deviceContext->CSSetUnorderedAccessViews(0, Util::NumElementsOf(uav), uav, 0);
		this->isCleared = true;
	}
}


