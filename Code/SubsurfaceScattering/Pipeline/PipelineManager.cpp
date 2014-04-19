#include "PipelineManager.h"
#include "RenderState\BlendState.h"
#include "RenderState\DepthStencilState.h"
#include "RenderState\RasterizerState.h"
#include "RenderState\SamplerState.h"
#include <d3dcompiler.h>
#include "..\Utilities\WindowShell.h"

using namespace Pipeline;

static PipelineManager* pipelineManagerInstance = 0;


PipelineManager& PipelineManager::Instance()
{
	if (!pipelineManagerInstance) pipelineManagerInstance = new PipelineManager();

	return *pipelineManagerInstance;
}
void PipelineManager::Release()
{
	ShaderStates::BlendStates::Release();
	ShaderStates::DepthStencilState::Release();
	ShaderStates::RasterizerState::Release();
	ShaderStates::SamplerState::Release();
	
	this->geometryPass.Release();
	this->d3dSwapchain->Release();
	this->renderTarget->Release();

	delete pipelineManagerInstance;
	pipelineManagerInstance = 0;

}
bool PipelineManager::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	this->device = device;
	this->deviceContext = deviceContext;

	if (!this->CreateSwapChain(width, height))			return false;
	if (!this->CreateRTV())					return false;

	this->geometryPass.Initiate(device, deviceContext, width, height, false);
	this->finalPass.Initiate(device, deviceContext, width, height, false);

	CreateViewport(width, height);

	return true;
}
void PipelineManager::ApplyGeometryPass(bool clearPrevious)
{
	//Clear pipeline
	if (clearPrevious)
	{

	}
	this->deviceContext->RSSetViewports(1, &this->viewPort);
	this->geometryPass.Apply();
}

void PipelineManager::Present()
{
	ID3D11RenderTargetView* rtv[] =
	{
		this->renderTarget,
		0,
		0,
		0,
	};

	this->deviceContext->OMSetRenderTargets(4, rtv, 0);
	ID3D11SamplerState *smp[] = { ShaderStates::SamplerState::GetLinear() };
	this->deviceContext->PSSetSamplers(0, 1, smp);
	
	ID3D11ShaderResourceView* srv[] =
	{
		this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_COLOR),
		0,
		0,
		0,
	};
	this->deviceContext->PSSetShaderResources(0, 4, srv);

	this->finalPass.Apply();

	this->d3dSwapchain->Present(0, 0);
}

PipelineManager::PipelineManager()
{
	
}
PipelineManager::~PipelineManager()
{

}



bool PipelineManager::CreateSwapChain(int width, int height)
{
	//generate static Swapchain Desc
	DXGI_SWAP_CHAIN_DESC desc;
	desc.OutputWindow = WindowShell::GetHWND();
	desc.BufferCount = 1;
	desc.Windowed = true;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = 0;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.Height = (UINT)height;
	desc.BufferDesc.Width = (UINT)width;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;


	//Get Device Factory
	::IDXGIDevice *dxgiDevice = NULL;
	if (FAILED(this->device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))
	{
		printf("Failed to Query for the GPU's dxgiDevice.\nFailed to create swapChain for the GPU.\n");
		return false;
	}

	::IDXGIAdapter *dxgiAdapter = NULL;
	if (FAILED(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter)))
	{
		dxgiDevice->Release();
		printf("Failed to get GPU's parent dxgiAdapter.\nFailed to create swapChain for the GPU.\n");
		return false;
	}
	dxgiDevice->Release();

	::IDXGIFactory *dxgiFactory = NULL;
	if (FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory)))
	{
		dxgiAdapter->Release();
		printf("Failed to get GPU's parent dxgiFactory.\nFailed to create swapChain for the GPU.\n");
		return false;
	}
	dxgiAdapter->Release();

	//Create SwapChain
	if (FAILED(dxgiFactory->CreateSwapChain(this->device, &desc, &this->d3dSwapchain)))
	{
		dxgiFactory->Release();
		printf("Failed to create swapChain for the GPU.\n");
		return false;
	}

	dxgiFactory->Release();

	return true;
}
bool PipelineManager::CreateRTV()
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descView.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descView.Texture2D.MipSlice = 0;

	ID3D11Texture2D* backBuffer;
	if (FAILED(this->d3dSwapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
	{
		printf("Failed to get BackBuffer from Swapchain");
		return false;
	}

	if (FAILED(this->device->CreateRenderTargetView(backBuffer, 0, &this->renderTarget)))
	{
		printf("Failed to create RTV for BackBuffer");
		backBuffer->Release();
		return false;
	}

	backBuffer->Release();

	return true;
}
void PipelineManager::CreateViewport(int width, int height)
{
	this->viewPort.TopLeftX = 0;
	this->viewPort.TopLeftY = 0;
	this->viewPort.Width = (FLOAT)width;
	this->viewPort.Height = (FLOAT)height;
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;
}

