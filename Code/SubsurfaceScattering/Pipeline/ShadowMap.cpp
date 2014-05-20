#include "ShadowMap.h"
#include "InputLayoutState.h"
#include "RenderState\DepthStencilState.h"
#include "RenderState\RasterizerState.h"
#include "RenderState\SamplerState.h"
#include "PipelineManager.h"
#include "..\Utilities\Util.h"
#include "..\Scene\Model.h"

struct ShadowSceneData
{
	SimpleMath::Matrix view;
	SimpleMath::Matrix projection;
};
struct ShadowObjectData
{
	SimpleMath::Matrix world;
};

ID3D11Device* ShadowMap_device = 0;
ID3D11DeviceContext* ShadowMap_dc = 0;
Shader *ShadowMap_depthShader = 0;
ID3D11Buffer* ShadowMap_sceneBuffer = 0;
ID3D11Buffer* ShadowMap_objectBuffer = 0;
int sharedCounter = 0;

bool CreateSharedData(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	if(!ShadowMap_depthShader)
	{
		ShadowMap_depthShader = new Shader();

		ShadowMap_device = device;
		ShadowMap_dc = deviceContext;

		if (!ShadowMap_depthShader->LoadCompiledShader("Shaders\\DepthShader.vertex.cso", ShaderType_VS, ShadowMap_device, ShadowMap_dc))
			return false;
	
		InputLayoutManager::MicrosoftFailedWithDirectX(device, ShadowMap_depthShader->GetByteCode(), ShadowMap_depthShader->GetByteCodeSize());
		ShadowMap_depthShader->RemoveByteCode();
	
		if (!ShaderStates::DepthStencilState::GetEnabledDepth(ShadowMap_device)) return false;
		if (!ShaderStates::RasterizerState::GetBackCullNoMS(ShadowMap_device)) return false;

		//Create Scene buffer
		D3D11_BUFFER_DESC bd;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth = sizeof(ShadowSceneData);
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = bd.BindFlags;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		if(FAILED(ShadowMap_device->CreateBuffer(&bd, 0, &ShadowMap_sceneBuffer)))
			return false;

		//Create Object buffer
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.ByteWidth = sizeof(ShadowObjectData);
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = bd.BindFlags;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		if(FAILED(ShadowMap_device->CreateBuffer(&bd, 0, &ShadowMap_objectBuffer)))
			return false;
	}
	sharedCounter++;
	return true;
}


#pragma region ShadowMap

	ShadowMap::ShadowMap() {  }
	ShadowMap::~ShadowMap() { }

	bool ShadowMap::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
	{

		if(!CreateSharedData(device, deviceContext))
			return false;

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
	void ShadowMap::Release()
	{ 
		sharedCounter--;
		if(sharedCounter == 0)
		{
			ShadowMap_depthShader->Release();
			delete ShadowMap_depthShader;
			ShadowMap_depthShader = 0;

			Util::SAFE_RELEASE( ShadowMap_sceneBuffer);
			Util::SAFE_RELEASE( ShadowMap_objectBuffer);
		}

		this->depth.Release(); 
	}
	void ShadowMap::Begin(const SimpleMath::Matrix& view, const SimpleMath::Matrix& p)
	{
		ShadowMap_dc->ClearDepthStencilView(this->depth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); 

		ShadowMap_depthShader->Apply();
		ShadowMap_dc->PSSetShader(0, 0, 0);

		ShadowMap_dc->IASetInputLayout(InputLayoutManager::GetLayout_V_VN_VT());
		ShadowMap_dc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
		ShadowMap_dc->OMSetDepthStencilState(ShaderStates::DepthStencilState::GetEnabledDepth(), 0);
		ShadowMap_dc->RSSetState(ShaderStates::RasterizerState::GetBackCullNoMS());
	
		ShadowMap_dc->RSSetViewports(1, &this->viewPort);
		ShadowMap_dc->OMSetRenderTargets(0, 0, this->depth);

		/**
		 *	This is for rendering linear values:
		 *	Check this: http://www.mvps.org/directx/articles/linear_z/linearz.htm
		 */
		SimpleMath::Matrix projection = p;
		float Q = projection._33;
		float N = -projection._43 / projection._33;
		float F = -N * Q / (1 - Q);
		projection._33 /= F;
		projection._43 /= F;

		D3D11_MAPPED_SUBRESOURCE res;
		if (SUCCEEDED(ShadowMap_dc->Map(ShadowMap_sceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
		{
			ShadowSceneData* data = (ShadowSceneData*)res.pData;
			data->projection = projection.Transpose();
			data->view = view.Transpose();
			ShadowMap_dc->Unmap(ShadowMap_sceneBuffer, 0);
		}

		ID3D11Buffer* buff[] =
		{
			ShadowMap_sceneBuffer,
		};
		ShadowMap_dc->VSSetConstantBuffers(1, 1, buff);
	}
	void ShadowMap::Draw( Model& m)
	{
		D3D11_MAPPED_SUBRESOURCE r;

		if( SUCCEEDED( ShadowMap_dc->Map( ShadowMap_objectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &r)))
		{
			ShadowObjectData* t = (ShadowObjectData*)r.pData;
			t->world = m.GetWorld();
			t->world = t->world.Transpose();

			ShadowMap_dc->Unmap(ShadowMap_objectBuffer, 0);
		}
		ShadowMap_dc->VSSetConstantBuffers(0, 1, &ShadowMap_objectBuffer);
		
		UINT off = 0;
		ShadowMap_dc->IASetVertexBuffers(0, 1, &m.GetMesh().vertexBuffer, &m.GetMesh().vertexStride, &off);
		ShadowMap_dc->Draw(m.GetMesh().vertexCount, 0);
	}
	void ShadowMap::End()
	{
		ShadowMap_dc->OMSetRenderTargets(0, NULL, NULL);
	}

	
	
#pragma endregion


#pragma region ShadowMapArray

	ShadowMapArray::ShadowMapArray()
	{

	}
	ShadowMapArray::~ShadowMapArray()
	{

	}
	bool ShadowMapArray::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, int depth)
	{
		if(!CreateSharedData(device, deviceContext))
			return false;

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
		texDesc.ArraySize = (UINT)depth;
		texDesc.MipLevels = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
	
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = 0;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.ArraySize = depth;
		dsvDesc.Texture2DArray.FirstArraySlice = 0;
		dsvDesc.Texture2DArray.MipSlice = 0;
	
		D3D11_SHADER_RESOURCE_VIEW_DESC	srvDesc;
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.ArraySize = (UINT)depth;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2DArray.MostDetailedMip = 0;

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
	void ShadowMapArray::Release()
	{

	}
	void ShadowMapArray::Clear(ID3D11DeviceContext* dc )
	{

	}
	void ShadowMapArray::Begin(const SimpleMath::Matrix& view, const SimpleMath::Matrix& projection, int shadowIndex)
	{

	}
	void ShadowMapArray::End()
	{

	}
	void ShadowMapArray::SetWorld(const SimpleMath::Matrix& world)
	{

	}

#pragma endregion