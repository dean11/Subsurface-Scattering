#include "ShadowMap.h"
#include "InputLayoutState.h"
#include "RenderState\DepthStencilState.h"
#include "RenderState\RasterizerState.h"
#include "RenderState\SamplerState.h"
#include "PipelineManager.h"
#include "..\Utilities\Util.h"
#include "..\Input.h"
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

UINT ShadowMapArray::width = 800;
UINT ShadowMapArray::height = 600;
UINT ShadowMapArray::depth = 1;
D3D11_VIEWPORT ShadowMapArray::viewPort = { 0, 0, 800, 600, 0, 1 };
bool ShadowMapArray::isCreated = false;
RenderSurface ShadowMapArray::depthSurface;

int ShadowMapUAV::ref = 0;
D3D11_VIEWPORT ShadowMapUAV::viewPort;
ID3D11ShaderResourceView* ShadowMapUAV::surface;

ID3D11Buffer* ShadowMapUAV::frameBuffer = 0;
DirectX::SimpleMath::Vector3 ShadowMapUAV::size = SimpleMath::Vector3(800.0f, 600.0f, 5.0f);
ID3D11Device* ShadowMapUAV::device = 0;

namespace 
{
	ID3D11Device* ShadowMap_device = 0;
	ID3D11DeviceContext* ShadowMap_dc = 0;
	Shader *ShadowMap_depthShader = 0;
	ID3D11Buffer* ShadowMap_sceneBuffer = 0;
	ID3D11Buffer* ShadowMap_objectBuffer = 0;
	int sharedCounter = 0;
}


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
		if(Input::IsKeyDown(VK_F8))
		{
			if (!Shader::CompileShaderToCSO("..\\Code\\SubsurfaceScattering\\Shaders\\DepthShader.header.hlsli", "Shaders\\DepthShader.header.cso", "fx_5_0", 0, 0, ShaderType_None, ShadowMap_device, ShadowMap_dc))
				printf("Failed to reload posteffect shader \"PostPass.header.hlsli\"\n");
			if (!ShadowMap_depthShader->CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\DepthShader.vertex.hlsl", "vs_5_0", 0, 0, ShaderType_VS, ShadowMap_device, ShadowMap_dc))
				printf("Failed to reload shadowmap shader \"DepthShader.vertex.hlsl\"\n");
		}

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
		 *	w = X scaling factor
		 *	h = Y scaling factor
		 *	N = near Z
		 *	F = far Z
		 *	Q = F / (F-N)
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
		this->index = -1;
	}
	ShadowMapArray::~ShadowMapArray()
	{

	}
	void SetSize(int width, int height);
	void SetSize(int width, int height, int depth);
	bool ShadowMapArray::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		if(!this->isCreated)
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
		texDesc.ArraySize = this->depth;
		texDesc.MipLevels = 1;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
	
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = 0;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
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

		if(!this->depthSurface.Create(sd, device))
			return false;
	#pragma endregion

		return true;
	}
	void ShadowMapArray::Release()
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

		this->depthSurface.Release(); 
	}
	void ShadowMapArray::Begin(const SimpleMath::Matrix view[], const SimpleMath::Matrix projection[], int size, int offset)
	{
		if(Input::IsKeyDown(VK_F8))
		{
			if (!Shader::CompileShaderToCSO("..\\Code\\SubsurfaceScattering\\Shaders\\DepthShader.header.hlsli", "Shaders\\DepthShader.header.cso", "fx_5_0", 0, 0, ShaderType_None, ShadowMap_device, ShadowMap_dc))
				printf("Failed to reload posteffect shader \"PostPass.header.hlsli\"\n");
			if (!ShadowMap_depthShader->CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\DepthShader.vertex.hlsl", "vs_5_0", 0, 0, ShaderType_VS, ShadowMap_device, ShadowMap_dc))
				printf("Failed to reload shadowmap shader \"DepthShader.vertex.hlsl\"\n");
		}

		ShadowMap_dc->ClearDepthStencilView(this->depthSurface, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); 

		ShadowMap_depthShader->Apply();
		ShadowMap_dc->PSSetShader(0, 0, 0);

		ShadowMap_dc->IASetInputLayout(InputLayoutManager::GetLayout_V_VN_VT());
		ShadowMap_dc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
		ShadowMap_dc->OMSetDepthStencilState(ShaderStates::DepthStencilState::GetEnabledDepth(), 0);
		ShadowMap_dc->RSSetState(ShaderStates::RasterizerState::GetBackCullNoMS());
	
		ShadowMap_dc->RSSetViewports(1, &this->viewPort);
		ShadowMap_dc->OMSetRenderTargets(0, 0, this->depthSurface);

		/**
		 *	This is for rendering linear values:
		 *	Check this: http://www.mvps.org/directx/articles/linear_z/linearz.htm
		 *	w = X scaling factor
		 *	h = Y scaling factor
		 *	N = near Z
		 *	F = far Z
		 *	Q = F / (F-N)
		 */
		//SimpleMath::Matrix projection = p;
		//float Q = projection._33;
		//float N = -projection._43 / projection._33;
		//float F = -N * Q / (1 - Q);
		//projection._33 /= F;
		//projection._43 /= F;
		//
		//D3D11_MAPPED_SUBRESOURCE res;
		//if (SUCCEEDED(ShadowMap_dc->Map(ShadowMap_sceneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
		//{
		//	ShadowSceneData* data = (ShadowSceneData*)res.pData;
		//	data->projection = projection.Transpose();
		//	data->view = view.Transpose();
		//	ShadowMap_dc->Unmap(ShadowMap_sceneBuffer, 0);
		//}
		//
		//ID3D11Buffer* buff[] =
		//{
		//	ShadowMap_sceneBuffer,
		//};
		//ShadowMap_dc->VSSetConstantBuffers(1, 1, buff);
	}
	void ShadowMapArray::Draw( Model& m)
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
	void ShadowMapArray::End()
	{
		ShadowMap_dc->OMSetRenderTargets(0, NULL, NULL);
	}

#pragma endregion



#pragma region ShadowMapUAV

	ShadowMapUAV::ShadowMapUAV()
	{
		this->index = -1;
	}
	ShadowMapUAV::~ShadowMapUAV()
	{
	}
	void ShadowMapUAV::SetSize(int width, int height)
	{
		int w = 0, h = 0;

		
	}
	void ShadowMapUAV::SetSize(int width, int height, int depth)
	{
		if(this->size == SimpleMath::Vector3((float)width, (float)height, (float)depth))
			return;


		this->size = SimpleMath::Vector3((float)width, (float)height, (float)depth);

		if(this->surface)	
		{
			this->surface->Release();
			ID3D11Device* dev = 0;
			this->surface->GetDevice(&dev);

			if(!this->Create(dev))
				printf("Could not create a surface!\n");

			dev->Release();
		}
	}
	bool ShadowMapUAV::Create(ID3D11Device* device)
	{
		this->device = device;
	#pragma region CreateSurface
		RenderSurface::TEXTURESURFACE_DESC sd;
		memset(&sd, 0, sizeof(RenderSurface::TEXTURESURFACE_DESC));

		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.Width = (UINT)this->size.x;
		texDesc.Height = (UINT)this->size.y;
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
		sd.width = (unsigned int)this->size.x;
		sd.height = (unsigned int)this->size.y;
		sd.depthDesc.dsvDesc = &dsvDesc;
		sd.depthDesc.dsvSrvDesc = &srvDesc;
		sd.depthDesc.dsvTexDesc = &texDesc;

		//if(!this->surface->.Create(sd, device))
		//	return false;
	#pragma endregion

		return true;

		ShadowMapUAV::ref ++;
		return true;
	}
	void ShadowMapUAV::Release()
	{
		ShadowMapUAV::ref --;

		if(ShadowMapUAV::ref == 0)
		{

		}
	}

#pragma endregion

