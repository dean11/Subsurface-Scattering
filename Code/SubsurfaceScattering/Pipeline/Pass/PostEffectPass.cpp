#include "PostEffectPass.h"

#include <d3dcompiler.h>
#include <D3DTK\SimpleMath.h>

#include "..\RenderState\DepthStencilState.h"
#include "..\RenderState\RasterizerState.h"
#include "..\RenderState\SamplerState.h"
#include "..\..\Utilities\Util.h"
#include "..\..\Input.h"

namespace 
{
	struct ShadowMapLightProxy
	{
		DirectX::XMFLOAT4X4 viewProjection;
		DirectX::XMFLOAT3 position;
		float range;
		DirectX::XMFLOAT3 direction;
		float spot;
		DirectX::XMFLOAT3 attenuation;
		DirectX::XMFLOAT3 color;

		float pad[2];
	};
	struct ConstBuffer
	{
		DirectX::XMFLOAT4 ambientLight;
		DirectX::XMFLOAT3 cameraPosition;

		int pointLightCount;
		int spotLightCount;
		int dirLightCount;
		int shadowCount;
		int sssEnabled;

		//float pad[1];
	};
}

PostEffectPass::PostEffectPass()
{
	this->device					= 0;
	this->deviceContext				= 0;
	this->backBufferUAV				= 0;
	this->pointLightBufferSRV		= 0;
	this->spotLightBufferSRV		= 0;
	this->dirLightBufferSRV			= 0;
	this->shadowBufferSRV			= 0;
	this->frameBuffer				= 0;
	this->shadowBuffer				= 0;
	this->lightBuffer				= 0;
	this->isCleared					= true;


	int temp = 0;
	this->maxPointLight = 50;
	this->maxDirLight = 5;
	this->maxSpotLight = 100;

	this->firstPointLight	= (temp += 0);
	this->firstSpotLight	= (temp += maxPointLight);
	this->firstDirLight		= (temp += maxSpotLight);
}
PostEffectPass::PostEffectPass(unsigned int maxPointLights, unsigned int maxDirectionalLights, unsigned int maxSpotLights)
{
	this->device					= 0;
	this->deviceContext				= 0;
	this->backBufferUAV				= 0;
	this->pointLightBufferSRV		= 0;
	this->spotLightBufferSRV		= 0;
	this->dirLightBufferSRV			= 0;
	this->shadowBufferSRV			= 0;
	this->frameBuffer				= 0;
	this->shadowBuffer				= 0;
	this->lightBuffer				= 0;
	this->isCleared					= true;


	this->maxPointLight = maxPointLights;
	this->maxDirLight = maxDirectionalLights;
	this->maxSpotLight = maxSpotLights;

	int temp = 0;
	this->firstPointLight	= (temp += 0);
	this->firstSpotLight	= (temp += this->maxPointLight);
	this->firstDirLight		= (temp += this->maxSpotLight);
}

void PostEffectPass::Release()
{
	this->device					= 0;
	this->deviceContext				= 0;
	Util::SAFE_RELEASE(this->backBufferUAV);
	Util::SAFE_RELEASE(this->pointLightBufferSRV);
	Util::SAFE_RELEASE(this->spotLightBufferSRV);
	Util::SAFE_RELEASE(this->dirLightBufferSRV);
	Util::SAFE_RELEASE(this->shadowBufferSRV);
	Util::SAFE_RELEASE(this->frameBuffer);
	Util::SAFE_RELEASE(this->shadowBuffer);
	Util::SAFE_RELEASE(this->lightBuffer);
	this->firstPointLight			= 0;
	this->firstDirLight				= 0;
	this->firstSpotLight			= 0;
	this->maxPointLight				= 0;
	this->maxDirLight				= 0;
	this->maxSpotLight				= 0;
	this->isCleared					= true;
	this->postShader.Release();
}
void PostEffectPass::Clear()
{
	if (!this->isCleared)
	{
		this->deviceContext->CSSetShader(0, 0, 0);

		ID3D11UnorderedAccessView* uav[] = { 0 };
		this->deviceContext->CSSetUnorderedAccessViews(0, Util::NumElementsOf(uav), uav, 0);

		ID3D11ShaderResourceView* nullSRV[SrvRegister_COUNT] = { 0 };
		this->deviceContext->CSSetShaderResources(0, SrvRegister_COUNT, nullSRV);

		this->isCleared = true;
	}
}
bool PostEffectPass::ReloadPostEffectShaders()
{
	UINT flag = 0;

#if defined (DEBUG) || defined (_DEBUG)
	flag |= D3DCOMPILE_DEBUG;
#endif
	if (!Shader::CompileShaderToCSO("..\\Code\\SubsurfaceScattering\\Shaders\\PostPass.header.hlsli", "Shaders\\PostPass.header.cso", "fx_5_0", flag, 0, ShaderType_CS, device, deviceContext))
		printf("Failed to reload posteffect shader \"PostPass.header.hlsli\"\n");
	if (!this->postShader.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\PostPass.compute.hlsl", "cs_5_0", flag, 0, ShaderType_CS, device, deviceContext))
		printf("Failed to reload posteffect shader \"PostPass.compute.hlsl\"\n");

	return true;
}
void PostEffectPass::Apply(const PostPassData& data)
{
	ID3D11ShaderResourceView* srv[SrvRegister_COUNT] = {0};
	srv[SrvRegister_Diffuse]				= data.diffuseMap;
	srv[SrvRegister_Normal]					= data.normalMap;
	srv[SrvRegister_Position]				= data.positionMap;
	srv[SrvRegister_Thickness]				= data.thicknessMap;
	srv[SrvRegister_PointLightBufferSRV]	= this->pointLightBufferSRV;
	srv[SrvRegister_SpotLightBufferSRV]		= this->spotLightBufferSRV;
	srv[SrvRegister_DirLightBufferSRV]		= this->dirLightBufferSRV;
	srv[SrvRegister_ShadowBufferSRV]		= this->shadowBufferSRV;

	ID3D11SamplerState* samp[] = 
	{ 
		ShaderStates::SamplerState::GetShadow(this->device), 
		ShaderStates::SamplerState::GetLinear(this->device),
	};

	
	this->UpdateConstantBuffer(data);
	this->UpdateShadowMaps(data, &srv[SrvRegister_shadowMap]);
	this->UpdateLights(data);
	

	this->postShader.Apply();
	this->deviceContext->CSSetSamplers(0, Util::NumElementsOf(samp), samp);
	this->deviceContext->CSSetShaderResources(0, Util::NumElementsOf(srv), srv);
	this->deviceContext->CSSetUnorderedAccessViews(0, 1, &this->backBufferUAV, 0);

	benchmarkClock.reset();

	this->deviceContext->Dispatch((unsigned int)((this->width + 31) / 32), (unsigned int)((this->height + 31) / 32), 1);



	this->isCleared = false;
}

bool PostEffectPass::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, IDXGISwapChain* swapChain)
{
	this->device = device;
	this->deviceContext = deviceContext;

	DXGI_SWAP_CHAIN_DESC d;
	swapChain->GetDesc(&d);
	this->width = d.BufferDesc.Width;
	this->height = d.BufferDesc.Height;

	ShaderStates::DepthStencilState::GetDisabledDepth(device);
	ShaderStates::RasterizerState::GetNoCullNoMs(device);
	ShaderStates::SamplerState::GetLinear(device);
	ShaderStates::SamplerState::GetPoint(device);
	
	if (!this->postShader.LoadCompiledShader("Shaders\\PostPass.compute.cso", ShaderType_CS, device, deviceContext))
		return false;
	
	if(!this->CreateBackBufferUAV(swapChain) )	return false;
	if(!this->CreateLightBuffer() )				return false;
	if(!this->CreateShadowBuffer() )			return false;
	
	return true;
}

void PostEffectPass::UpdateConstantBuffer(const PostPassData& data)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	if (SUCCEEDED(this->deviceContext->Map(this->frameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData)))
	{
		ConstBuffer* b = (ConstBuffer*)mappedData.pData;
		b->dirLightCount = data.dirCount;
		b->pointLightCount = data.pointCount;
		b->spotLightCount = data.spotCount;
		b->ambientLight = DirectX::XMFLOAT4(data.ambientLight.x, data.ambientLight.y, data.ambientLight.z, 0.0f);
		b->shadowCount = min(data.shadowCount, maxShadowMaps);
		b->shadowCount = max(b->shadowCount, 0);
		b->sssEnabled = Input::IsKeyDown(VK_P) ? 1 : 0;
		b->cameraPosition = data.cameraPos;
		this->deviceContext->Unmap(this->frameBuffer, 0);
		this->deviceContext->CSSetConstantBuffers(0, 1, &this->frameBuffer);
	}
}
void PostEffectPass::UpdateShadowMaps(const PostPassData& data, ID3D11ShaderResourceView* srv[])
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	if ( (data.shadowCount != 0) && SUCCEEDED(this->deviceContext->Map(this->shadowBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData)))
	{
		ShadowMapLightProxy* s = (ShadowMapLightProxy*)mappedData.pData;
		for (int i = 0; i < this->maxShadowMaps && i < data.shadowCount; i++)
		{
			srv[i]					= data.shadowData[i]->shadowMap;
			s[i].attenuation		= data.shadowData[i]->attenuation;
			s[i].color				= data.shadowData[i]->color;
			s[i].spot				= data.shadowData[i]->spot;
			s[i].direction			= data.shadowData[i]->camera.GetForward();
			s[i].position			= data.shadowData[i]->camera.GetPosition();
			s[i].range				= data.shadowData[i]->range;

			/**
			 * This is for rendering linear values:
			 * Check this: http://www.mvps.org/directx/articles/linear_z/linearz.htm
			 */
			//SimpleMath::Matrix v = XMLoadFloat4x4(& data.shadowData[i]->camera.GetViewMatrix() );
			//SimpleMath::Matrix p = XMLoadFloat4x4(& data.shadowData[i]->camera.GetProjectionMatrix() );
			//float Q = p._33;
			//float N = -p._43 / p._33;
			//float F = -N * Q / (1 - Q);
			//p._33 /= F;
			//p._43 /= F;
			//s[i].viewProjection =  (v * p).Transpose(  );

			SimpleMath::Matrix v = XMLoadFloat4x4(& data.shadowData[i]->camera.GetViewMatrix() );
			SimpleMath::Matrix p = XMLoadFloat4x4(& data.shadowData[i]->camera.GetProjectionMatrix() );
			SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(0.5f, -0.5f, 1.0f);
			SimpleMath::Matrix translation = SimpleMath::Matrix::CreateTranslation(0.5f, 0.5f, 0.0f);
    		
			s[i].viewProjection = (v * p * scale * translation).Transpose();
		}
		this->deviceContext->Unmap(this->shadowBuffer, 0);
	}
}
void PostEffectPass::UpdateLights(const PostPassData& data)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	if (((data.dirCount + data.pointCount + data.spotCount) != 0) && SUCCEEDED(this->deviceContext->Map(this->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData)))
	{
		BasicLightData::PointLight* point	= &((BasicLightData::PointLight*) mappedData.pData)[this->firstPointLight];
		BasicLightData::Spotlight* spot		= ((BasicLightData::Spotlight*)(&point[this->firstSpotLight]));
		BasicLightData::Directional* dir	= ((BasicLightData::Directional*)(&point[this->firstDirLight]));
		
		memcpy(point, data.pointData, sizeof(BasicLightData::PointLight) * data.pointCount);
		memcpy(spot, data.spotData, sizeof(BasicLightData::Spotlight) * data.spotCount);
		memcpy(dir, data.dirData, sizeof(BasicLightData::Directional) * data.dirCount);

		this->deviceContext->Unmap(this->lightBuffer, 0);
	}
}
	
bool PostEffectPass::CreateLightBuffer()
{
	HRESULT hr = S_OK;
	D3D11_BUFFER_DESC bDesc;
	bDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bDesc.ByteWidth = (sizeof(BasicLightData::Spotlight) * (this->maxDirLight + this->maxPointLight + this->maxSpotLight));
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bDesc.StructureByteStride = sizeof(BasicLightData::Spotlight);
	bDesc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(this->device->CreateBuffer(&bDesc, 0, &this->lightBuffer))) 
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC sbsrv;
	sbsrv.Format = DXGI_FORMAT_UNKNOWN;
	sbsrv.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

	sbsrv.Buffer.ElementOffset = this->firstPointLight;
	sbsrv.Buffer.ElementWidth = this->maxPointLight;
	if (FAILED(this->device->CreateShaderResourceView(this->lightBuffer, &sbsrv, &this->pointLightBufferSRV)))
		return false;	

	sbsrv.Buffer.ElementOffset = this->firstSpotLight;
	sbsrv.Buffer.ElementWidth = this->maxSpotLight;
	if (FAILED(this->device->CreateShaderResourceView(this->lightBuffer, &sbsrv, &this->spotLightBufferSRV)))
		return false;

	sbsrv.Buffer.ElementOffset = this->firstDirLight;
	sbsrv.Buffer.ElementWidth = this->maxDirLight;
	if (FAILED(this->device->CreateShaderResourceView(this->lightBuffer, &sbsrv, &this->dirLightBufferSRV)))
		return false;

	//Create constant buffer
	D3D11_BUFFER_DESC lBdesc;
	lBdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lBdesc.ByteWidth = sizeof(ConstBuffer);
	lBdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lBdesc.MiscFlags = 0;
	lBdesc.StructureByteStride = sizeof(ConstBuffer);
	lBdesc.Usage = D3D11_USAGE_DYNAMIC;
	if (FAILED(this->device->CreateBuffer(&lBdesc, 0, &this->frameBuffer)))
		return false;
	
	return true;
}
bool PostEffectPass::CreateShadowBuffer()
{
	D3D11_BUFFER_DESC shadowDesc;
	shadowDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.ByteWidth = (sizeof(ShadowMapLightProxy) * this->maxShadowMaps);
	shadowDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	shadowDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	shadowDesc.StructureByteStride = sizeof(ShadowMapLightProxy);
	shadowDesc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(this->device->CreateBuffer(&shadowDesc, 0, &this->shadowBuffer))) 
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC shadowSrvDesc;
	shadowSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	shadowSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shadowSrvDesc.Buffer.ElementOffset = 0;
	shadowSrvDesc.Buffer.ElementWidth = this->maxShadowMaps;
	if (FAILED(this->device->CreateShaderResourceView(this->shadowBuffer, &shadowSrvDesc, &this->shadowBufferSRV)))
		return false;	

	return true;
}
bool PostEffectPass::CreateBackBufferUAV(IDXGISwapChain* swap)
{
	HRESULT hr = S_OK;
	ID3D11Texture2D *bb;
	if (FAILED(hr = swap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&bb))))
	{
		printf("Failed to get BackBuffer from Swapchain");
		return false;
	}

	if (FAILED(hr = this->device->CreateUnorderedAccessView(bb, 0, &this->backBufferUAV)))
	{
		bb->Release();
		printf("Failed to create unordered access view with back buffer!");
		return false;
	}

	bb->Release();
	return true;
}

