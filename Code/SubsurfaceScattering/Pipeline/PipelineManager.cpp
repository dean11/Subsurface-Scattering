#include "PipelineManager.h"
#include "RenderState\BlendState.h"
#include "RenderState\DepthStencilState.h"
#include "RenderState\RasterizerState.h"
#include "RenderState\SamplerState.h"
#include "InputLayoutState.h"
#include <d3dcompiler.h>
#include "..\Utilities\WindowShell.h"

using namespace Pipeline;

static PipelineManager* pipelineManagerInstance = 0;

struct ObjectMatrixData
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInversTranspose;
};
struct SceneMatrixData
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};


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
	InputLayoutManager::Release();
	
	this->geometryPass.Release();
	this->finalPass.Release();
	this->lightPass.Release();

	if (this->d3dSwapchain)			this->d3dSwapchain->Release();			this->d3dSwapchain = 0;
	if (this->renderTarget)			this->renderTarget->Release();			this->renderTarget = 0;
	if (this->objectMatrixBuffer)	this->objectMatrixBuffer->Release();	this->objectMatrixBuffer = 0;
	if (this->sceneMatrixBuffer)	this->sceneMatrixBuffer->Release();		this->sceneMatrixBuffer = 0;

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
	this->lightPass.Initiate(device, deviceContext, width, height, false);
	this->finalPass.Initiate(device, deviceContext, width, height, false);
	
	CreateViewport(width, height);
	this->CreateConstantBuffers();

	return true;
}

void PipelineManager::ApplyGeometryPass()
{
	if (this->prevPass) this->prevPass->Clear();
	

	this->geometryPass.Apply();
	this->deviceContext->RSSetViewports(1, &this->viewPort);

	ID3D11Buffer* buff[] =
	{
		this->sceneMatrixBuffer,
	};
	this->deviceContext->VSSetConstantBuffers(1, 1, buff);
	this->prevPass = &this->geometryPass;
}

void PipelineManager::ApplyLightPass(const LightData& data)
{
	if (this->prevPass) this->prevPass->Clear();

	this->lightPass.Apply(this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_COLOR), this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_NORMAL));
	this->lightPass.RenderPointLight(data.pointData, data.pointCount);
	this->lightPass.RenderDirectionalLight(data.dirData, data.dirCount);
	this->lightPass.RenderSpotLight(data.spotData, data.spotCount);

	this->prevPass = &this->lightPass;
}

void PipelineManager::Present()
{
	if (this->prevPass) this->prevPass->Clear();

	this->deviceContext->RSSetViewports(1, &this->viewPort);

	ID3D11RenderTargetView* rtv[] = { this->renderTarget };
	this->deviceContext->OMSetRenderTargets(1, rtv, 0);
	
	ID3D11ShaderResourceView* srv[] =
	{
		this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_NORMAL),
		this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_COLOR),
		this->lightPass.GetLightMapSRV(),
		0,
	};

	this->deviceContext->PSSetShaderResources(0, 3, srv);
	this->finalPass.Apply();

	this->d3dSwapchain->Present(0, 0);

	this->prevPass = &this->finalPass;
}

void PipelineManager::SetObjectMatrixBuffers(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& worldInversTranspose)
{
	D3D11_MAPPED_SUBRESOURCE res;
	if (SUCCEEDED(this->deviceContext->Map(this->objectMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
	{
		ObjectMatrixData* data = (ObjectMatrixData*)res.pData;
		DirectX::XMStoreFloat4x4(&data->world, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&world)));
		data->worldInversTranspose = worldInversTranspose;
		this->deviceContext->Unmap(this->objectMatrixBuffer, 0);
	}

	ID3D11Buffer* buff[] =
	{
		this->objectMatrixBuffer,
	};
	this->deviceContext->VSSetConstantBuffers(0, 1, buff);
}
void PipelineManager::SetSceneMatrixBuffers(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	D3D11_MAPPED_SUBRESOURCE res;
	if (SUCCEEDED(this->deviceContext->Map(this->sceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
	{
		SceneMatrixData* data = (SceneMatrixData*)res.pData;
		DirectX::XMStoreFloat4x4(&data->projection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&projection)));
		DirectX::XMStoreFloat4x4(&data->view, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&view)));
		this->deviceContext->Unmap(this->sceneMatrixBuffer, 0);
	}
}

PipelineManager::PipelineManager()
{
	this->prevPass = 0;
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
bool PipelineManager::CreateConstantBuffers()
{
	D3D11_BUFFER_DESC obj;
	obj.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	obj.ByteWidth = sizeof(ObjectMatrixData);
	obj.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	obj.MiscFlags = 0;
	obj.StructureByteStride = sizeof(ObjectMatrixData);
	obj.Usage = D3D11_USAGE_DYNAMIC;
	if (FAILED(this->device->CreateBuffer(&obj, 0, &this->objectMatrixBuffer)))
		return false;

	obj.ByteWidth = sizeof(SceneMatrixData);
	obj.StructureByteStride = sizeof(SceneMatrixData);
	if (FAILED(this->device->CreateBuffer(&obj, 0, &this->sceneMatrixBuffer)))
		return false;

	return true;
}
