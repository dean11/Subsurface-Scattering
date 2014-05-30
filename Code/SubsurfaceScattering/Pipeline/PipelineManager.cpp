#include "PipelineManager.h"
#include "RenderState\BlendState.h"
#include "RenderState\DepthStencilState.h"
#include "RenderState\RasterizerState.h"
#include "RenderState\SamplerState.h"
#include "InputLayoutState.h"
#include <d3dcompiler.h>
#include "..\Utilities\WindowShell.h"
#include "..\Utilities\TextRender.h"
#include "..\Utilities\Util.h"
#include "..\Input.h"

using namespace Pipeline;
struct winRectangle :public RECT
{
	winRectangle(LONG x, LONG y, LONG w, LONG h)
	{
		this->left = x;
		this->top = y;
		this->right = x+w;
		this->bottom = y+h;
	}
};
static PipelineManager* pipelineManagerInstance = 0;
static std::vector<ID3D11ShaderResourceView*> shadowMapDebug;


struct ObjectMatrixData
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInversTranspose;
	int nrOfMatLayers;

	float pad[3];
};
struct SceneMatrixData
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	int nrOfShadowMaps;
	int sssEnable; //First block has number of layers.
	
	float pad[2];
};
struct DepthPointLightData
{
	DirectX::XMFLOAT4 posRange;
};

struct DepthSpotlightData
{
	//TBD
};

struct DepthDirLightData
{
	//TBD
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
	//this->finalPass.Release();
	//this->lightPass.Release();
	//this->sssPass.Release();
	this->postPass.Release();

	Util::SAFE_RELEASE(this->d3dSwapchain);
	Util::SAFE_RELEASE(this->renderTarget);
	Util::SAFE_RELEASE(this->objectMatrixBuffer);
	Util::SAFE_RELEASE(this->sceneMatrixBuffer);
	Util::SAFE_RELEASE(this->depthPointLightBuffer);

	TextRender::Release();

	delete this->spriteBatch;
	this->spriteBatch = 0;

	delete pipelineManagerInstance;

	pipelineManagerInstance = 0;
}
bool PipelineManager::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	this->width = width;
	this->height = height;
	this->device = device;
	this->deviceContext = deviceContext;

	this->spriteBatch = new DirectX::SpriteBatch(deviceContext);

	if (!this->CreateSwapChain(width, height))			return false;
	if (!this->CreateRTV())								return false;
	
	if(!this->geometryPass.Initiate(device, deviceContext, width, height, false)) return false;
	//if(!this->lightPass.Initiate(device, deviceContext, width, height, false)) return false;
	//if(!this->finalPass.Initiate(device, deviceContext, width, height, false, this->d3dSwapchain)) return false;
	//if(!this->sssPass.Initiate(device, deviceContext, width, height)) return false;
	if(!this->postPass.Initiate(device, deviceContext, this->d3dSwapchain)) return false;

	CreateViewport(width, height);
	this->CreateConstantBuffers();
	CreateTranslucentBuffer(20);

	return true;
}


void PipelineManager::ApplyGeometryPass(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, BasicLightData::ShadowMapLight*const* shadowData, int shadowCount)
{
	if (this->prevPass) this->prevPass->Clear();
	
	this->geometryPass.Apply(shadowData, shadowCount);
	this->deviceContext->RSSetViewports(1, &this->viewPort);

	D3D11_MAPPED_SUBRESOURCE res;
	if (SUCCEEDED(this->deviceContext->Map(this->sceneMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
	{
		SceneMatrixData* data = (SceneMatrixData*)res.pData;
		DirectX::XMStoreFloat4x4(&data->projection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&projection)));
		DirectX::XMStoreFloat4x4(&data->view, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&view)));
		data->nrOfShadowMaps = shadowCount;
		data->sssEnable = Input::IsKeyDown(VK_P) ? 1 : 0;
		this->deviceContext->Unmap(this->sceneMatrixBuffer, 0);
	}

	ID3D11Buffer* buff[] =
	{
		this->sceneMatrixBuffer,
	};
	this->deviceContext->VSSetConstantBuffers(1, 1, buff);
	this->deviceContext->PSSetConstantBuffers(1, 1, buff);
	
	this->prevPass = &this->geometryPass;
}
void PipelineManager::ApplyPostEffectPass(const LightPass::LightData& light )
{
	if (this->prevPass) this->prevPass->Clear();

	if(Input::IsKeyDown(VK_L))
		this->postPass.ReloadPostEffectShaders();

	if(this->debugRTV)
	{
		shadowMapDebug.resize(0);
		for (size_t i = 0; i < (size_t)light.shadowCount; i++)
			shadowMapDebug.push_back(light.shadowData[i]->shadowMap);
	}

	PostEffectPass::PostPassData post;
	post.ambientLight	= light.ambientLight;
	post.diffuseMap		= this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_COLOR);
	post.positionMap	= this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_POSITION);
	post.thicknessMap	= this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_THICKNESS);
	post.normalMap		= this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_NORMAL);
	post.dirCount		= light.dirCount;
	post.dirData		= light.dirData;
	post.pointCount		= light.pointCount;
	post.pointData		= light.pointData;
	post.spotCount		= light.spotCount;
	post.spotData		= light.spotData;
	post.shadowCount	= light.shadowCount;
	post.shadowData		= light.shadowData;
	post.view			= light.view;
	post.projection		= light.proj;
	post.cameraPos		= light.cameraPos;
	

	this->postPass.Apply(post);
	
	this->prevPass = &this->postPass;
}
void PipelineManager::ApplyUIPass(ID3D11ShaderResourceView*const* srv, int total)
{
	if(this->prevPass) this->prevPass->Clear();

	//Render ui stuff
	this->deviceContext->RSSetViewports(1, &this->viewPort);
	ID3D11RenderTargetView* rtv[] = { this->renderTarget };
	this->deviceContext->OMSetRenderTargets(1, rtv, 0);

	for (int i = 0; i < total; i++)
	{
		shadowMapDebug.push_back(srv[i]);
	}

	this->spriteBatch->Begin();
	{
		if (this->debugRTV)
		{
			int off = 0;
			int size = 150;
			this->spriteBatch->Draw(this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_NORMAL), winRectangle((off), 0, size, size));
			this->spriteBatch->Draw(this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_POSITION), winRectangle((off += size+1), 0, size, size));
			this->spriteBatch->Draw(this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_COLOR), winRectangle((off += size+1), 0, size, size));
			this->spriteBatch->Draw(this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_THICKNESS), winRectangle((off += size+1), 0, size, size));
			this->spriteBatch->Draw(this->geometryPass.GetShaderResource(Pipeline::GBuffer_RTV_Layout_DepthStencil), winRectangle((off += size+1), 0, size, size));
			
			size = 123;
			off = -size;
			for (size_t i = 0; i < shadowMapDebug.size(); i++)
				this->spriteBatch->Draw(shadowMapDebug[i], winRectangle((off += size+1), height - size, size, size));
		}
	
		//Draw some text
		TextRender::Present(this->spriteBatch);
	
	}this->spriteBatch->End();

	ID3D11RenderTargetView* nullrtv[] = { 0 };
	this->deviceContext->OMSetRenderTargets(Util::NumElementsOf(nullrtv), nullrtv, 0);
	ID3D11ShaderResourceView* nullsrv[PostEffectPass::SrvRegister_COUNT] = {0};
	this->deviceContext->PSSetShaderResources(0, Util::NumElementsOf(nullsrv), nullsrv );

	shadowMapDebug.resize(0);
}
void PipelineManager::Present()
{
	this->d3dSwapchain->Present(0, 0);
}


void PipelineManager::SetMeshBuffer(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& worldInversTranspose, DirectX::XMFLOAT4 const* layers, int layerCount)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	if ( (layerCount > 0) && SUCCEEDED(this->deviceContext->Map(this->objectTranslucentBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData)))
	{
		DirectX::XMFLOAT4* s = (DirectX::XMFLOAT4*)mappedData.pData;
		for (int i = 0; i < layerCount; i++)
		{
			s[i] = layers[i];
		}
		this->deviceContext->Unmap(this->objectTranslucentBuffer, 0);
	}
	this->deviceContext->PSSetShaderResources(20, 1, &this->objectTranslucentSrv);


	SimpleMath::Matrix w = DirectX::XMLoadFloat4x4(& world );
	SimpleMath::Matrix wit = DirectX::XMLoadFloat4x4( &worldInversTranspose );
	D3D11_MAPPED_SUBRESOURCE res;
	if (SUCCEEDED(this->deviceContext->Map(this->objectMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
	{
		ObjectMatrixData* data = (ObjectMatrixData*)res.pData;
		data->world = w.Transpose();
		data->worldInversTranspose = wit.Transpose();
		data->nrOfMatLayers = layerCount;
		this->deviceContext->Unmap(this->objectMatrixBuffer, 0);
	}

	ID3D11Buffer* buff[] =
	{
		this->objectMatrixBuffer,
	};
	this->deviceContext->VSSetConstantBuffers(0, 1, buff);
	this->deviceContext->PSSetConstantBuffers(0, 1, buff);
}


PipelineManager::PipelineManager()
{
	this->prevPass = 0;
	this->debugRTV = true;
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
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
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

	obj.ByteWidth = sizeof(DepthPointLightData);
	obj.StructureByteStride = sizeof(DepthPointLightData);
	if (FAILED(this->device->CreateBuffer(&obj, 0, &this->depthPointLightBuffer)))
		return false;

	return true;
}
bool PipelineManager::CreateTranslucentBuffer(int width)
{
	D3D11_BUFFER_DESC shadowDesc;
	shadowDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	
	shadowDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	shadowDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	shadowDesc.StructureByteStride =  (sizeof(float) * 4);
	shadowDesc.ByteWidth = shadowDesc.StructureByteStride * width;
	shadowDesc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(this->device->CreateBuffer(&shadowDesc, 0, &this->objectTranslucentBuffer))) 
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC shadowSrvDesc;
	shadowSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	shadowSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shadowSrvDesc.Buffer.ElementOffset = 0;
	shadowSrvDesc.Buffer.ElementWidth = width;
	if (FAILED(this->device->CreateShaderResourceView(this->objectTranslucentBuffer, &shadowSrvDesc, &this->objectTranslucentSrv)))
		return false;	

	return true;
}

