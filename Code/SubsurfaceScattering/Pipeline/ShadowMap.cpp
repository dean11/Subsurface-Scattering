#include "ShadowMap.h"
#include "InputLayoutState.h"
#include "RenderState\DepthStencilState.h"
#include "RenderState\RasterizerState.h"
#include "RenderState\SamplerState.h"
#include "PipelineManager.h"

struct LightSceneData
{
	SimpleMath::Matrix view;
	SimpleMath::Matrix projection;
};

ID3D11Device* ShadowMap::device = 0;
ID3D11DeviceContext* ShadowMap::dc = 0;
Shader *ShadowMap::depthShader = 0;
ID3D11Buffer* ShadowMap::sceneBuffer = 0;

ShadowMap::ShadowMap() {  }
ShadowMap::~ShadowMap() { }

bool ShadowMap::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	if(!this->depthShader)
	{
		this->depthShader = new Shader();

		this->device = device;
		this->dc = deviceContext;

		if (!this->depthShader->LoadCompiledShader("Shaders\\DepthShader.vertex.cso", ShaderType_VS, this->device, this->dc))
			return false;
	
		InputLayoutManager::MicrosoftFailedWithDirectX(device, this->depthShader->GetByteCode(), this->depthShader->GetByteCodeSize());
		this->depthShader->RemoveByteCode();

		if (!ShaderStates::DepthStencilState::GetEnabledDepth(this->device)) return false;
		if (!ShaderStates::RasterizerState::GetBackCullNoMS(this->device)) return false;

		D3D11_BUFFER_DESC bd;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth = sizeof(LightSceneData);
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = bd.BindFlags;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		if(FAILED(this->device->CreateBuffer(&bd, 0, &this->sceneBuffer)))
			return false;
	}

	// Viewport that matches the shadow map dimensions
	this->viewPort.TopLeftX = 0;
	this->viewPort.TopLeftY = 0;
	this->viewPort.MinDepth = 0;
	this->viewPort.MaxDepth = 1;
	this->viewPort.Width = (float)width;
	this->viewPort.Height = (float)height;

#pragma region CreateSurface
	RenderSurface::TEXTURESURFACE_DESC sd;
	memset(&sd, 0, sizeof(RenderSurface::TEXTURESURFACE_DESC));

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = (UINT)width;
	texDesc.Height = (UINT)height;
	texDesc.CPUAccessFlags = 0;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC	srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	sd.useColor = false;
	sd.useDepth = true;
	sd.width = (unsigned int)width;
	sd.height = (unsigned int)height;
	sd.depthDesc.dsvDesc = &dsvDesc;
	sd.depthDesc.dsvSrvDesc = &srvDesc;
	sd.depthDesc.dsvTexDesc = &texDesc;

	if(!this->depth.Create(sd, device))
		return false;
#pragma endregion

	return true;
}
bool ShadowMap::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, const SimpleMath::Matrix& view, const SimpleMath::Matrix& projection )
{
	this->view = view;
	this->projection = projection;
	return	this->Create(device, deviceContext, width, height);
}
void ShadowMap::Release()
{ 
	delete this->depthShader;
	this->depthShader = 0;

	this->depth.Release(); 
}
void ShadowMap::Clear(ID3D11DeviceContext* dc )
{ 
	dc->ClearDepthStencilView(this->depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); 
}
	
void ShadowMap::Begin(const SimpleMath::Matrix& view, const SimpleMath::Matrix& p)
{
	this->depthShader->Apply();
	this->dc->PSSetShader(0, 0, 0);

	this->dc->IASetInputLayout(InputLayoutManager::GetLayout_V_VN_VT());
	this->dc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	this->dc->OMSetDepthStencilState(ShaderStates::DepthStencilState::GetEnabledDepth(), 0);
	this->dc->RSSetState(ShaderStates::RasterizerState::GetBackCullNoMS());
	
	UINT i = 1;
	this->dc->RSGetViewports(&i, &this->prevViewport);

	this->dc->RSSetViewports(1, &this->viewPort);
	this->dc->OMSetRenderTargets(0, 0, this->depth);

	/**
     * This is for rendering linear values:
     * Check this: http://www.mvps.org/directx/articles/linear_z/linearz.htm
     */
    SimpleMath::Matrix projection = p;
    float Q = projection._33;
    float N = -projection._43 / projection._33;
    float F = -N * Q / (1 - Q);
    projection._33 /= F;
    projection._43 /= F;

	D3D11_MAPPED_SUBRESOURCE res;
	if (SUCCEEDED(this->dc->Map(this->sceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
	{
		LightSceneData* data = (LightSceneData*)res.pData;
		DirectX::XMStoreFloat4x4(&data->projection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&projection)));
		DirectX::XMStoreFloat4x4(&data->view, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&view)));
		this->dc->Unmap(this->sceneBuffer, 0);
	}

	ID3D11Buffer* buff[] =
	{
		this->sceneBuffer,
	};
	this->dc->VSSetConstantBuffers(1, 1, buff);
}
void ShadowMap::Begin()
{
	this->depthShader->Apply();
	this->dc->PSSetShader(0, 0, 0);

	this->dc->IASetInputLayout(InputLayoutManager::GetLayout_V_VN_VT());
	this->dc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	this->dc->OMSetDepthStencilState(ShaderStates::DepthStencilState::GetEnabledDepth(), 0);
	this->dc->RSSetState(ShaderStates::RasterizerState::GetBackCullNoMS());
	
	UINT i = 1;
	this->dc->RSGetViewports(&i, &this->prevViewport);

	this->dc->RSSetViewports(1, &this->viewPort);
	this->dc->OMSetRenderTargets(0, 0, this->depth);

	/**
     * This is for rendering linear values:
     * Check this: http://www.mvps.org/directx/articles/linear_z/linearz.htm
     */
    SimpleMath::Matrix projection = this->projection;
    float Q = projection._33;
    float N = -projection._43 / projection._33;
    float F = -N * Q / (1 - Q);
    projection._33 /= F;
    projection._43 /= F;

	D3D11_MAPPED_SUBRESOURCE res;
	if (SUCCEEDED(this->dc->Map(this->sceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
	{
		LightSceneData* data = (LightSceneData*)res.pData;
		DirectX::XMStoreFloat4x4(&data->projection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&projection)));
		DirectX::XMStoreFloat4x4(&data->view, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&this->view)));
		this->dc->Unmap(this->sceneBuffer, 0);
	}

	ID3D11Buffer* buff[] =
	{
		this->sceneBuffer,
	};
	this->dc->VSSetConstantBuffers(1, 1, buff);
}
void ShadowMap::End()
{
	this->dc->RSSetViewports(1, &this->prevViewport);
    this->dc->OMSetRenderTargets(0, NULL, NULL);
}

void ShadowMap::SetWorld(const SimpleMath::Matrix& world)
{
	Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(world, world.Invert());
}
void ShadowMap::SetView(const SimpleMath::Matrix& m)
{
	this->view = m;
}
void ShadowMap::SetProjection(const SimpleMath::Matrix& m)
{
	this->projection = m;
}


