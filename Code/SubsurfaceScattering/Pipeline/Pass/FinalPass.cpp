#include "FinalPass.h"
#include "..\RenderState\DepthStencilState.h"
#include "..\RenderState\RasterizerState.h"
#include "..\RenderState\SamplerState.h"
#include "..\InputLayoutState.h"
#include "..\Vertex.h"

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
	this->quadIBuffer->Release();
	this->quadVBuffer->Release();
	this->vertex.Release();
	this->pixel.Release();
}
void FinalPass::Apply()
{
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->IASetInputLayout(InputLayoutManager::GetLayout_V_VN_VT());
	this->deviceContext->OMSetDepthStencilState(ShaderStates::DepthStencilState::GetDisabledDepth(), 0);
	this->deviceContext->RSSetState(ShaderStates::RasterizerState::GetNoCullNoMs());
	ID3D11SamplerState *smp[] = { ShaderStates::SamplerState::GetLinear() };
	this->deviceContext->PSSetSamplers(0, 1, smp);

	UINT elemSize = sizeof(Vertex);
	UINT off = 0;
	this->deviceContext->IASetIndexBuffer(this->quadIBuffer, DXGI_FORMAT_R32_UINT, 0);
	this->deviceContext->IASetVertexBuffers(0, 1, &this->quadVBuffer, &elemSize, &off);

	this->vertex.Apply();
	this->pixel.Apply();

	this->deviceContext->DrawIndexed(6, 0, 0);
}
bool FinalPass::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool foreShaderCompile)
{
	HRESULT hr = S_OK;
	this->device = device;
	this->deviceContext = deviceContext;

	ShaderStates::DepthStencilState::GetDisabledDepth(device);
	ShaderStates::RasterizerState::GetNoCullNoMs(device);
	ShaderStates::SamplerState::GetLinear(device);

	if (foreShaderCompile)
	{
		UINT flag = 0;

		#if defined(DEBUG) || defined(_DEBUG)
			flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
		#endif

		if (!this->vertex.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\FinalShader.vertex.hlsl", "vs_5_0", flag, 0, ShaderType_VS, device, deviceContext))
			return false;

		if (!this->pixel.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\FinalShader.pixel.hlsl", "ps_5_0", flag, 0, ShaderType_PS, device, deviceContext))
			return false;
	}
	else
	{
		if (!this->vertex.LoadCompiledShader("Shaders\\FinalShader.vertex.cso", ShaderType_VS, device, deviceContext))
			return false;

		if (!this->pixel.LoadCompiledShader("Shaders\\FinalShader.pixel.cso", ShaderType_PS, device, deviceContext))
			return false;
	}

	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(Vertex) * 4;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = (sizeof(Vertex));
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	
	Vertex vert[4] =
	{
		{ //[0]
			DirectX::XMFLOAT3(-1.0f, +1.0f, 0.0f),		//COORD
			DirectX::XMFLOAT3(),			//PADD
			DirectX::XMFLOAT2(+0.0f, +0.0f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		},
		{ //[1]
			DirectX::XMFLOAT3(+1.0f, +1.0f, 0.0f),		//COORD
			DirectX::XMFLOAT3(),			//PADD
			DirectX::XMFLOAT2(+1.0f, +0.0f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		},
		{ //[2]
			DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f),		//COORD
			DirectX::XMFLOAT3(),			//PADD
			DirectX::XMFLOAT2(+0.0f, +1.0f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		},
		{ //[3]
			DirectX::XMFLOAT3(+1.0f, -1.0f, 0.0f),		//COORD
			DirectX::XMFLOAT3(),			//PADD
			DirectX::XMFLOAT2(+1.0f, +1.0f),				//UV
			DirectX::XMFLOAT3(),
			DirectX::XMFLOAT3(),
		}
	};

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vert;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	if (FAILED(hr = device->CreateBuffer(&desc, &data, &this->quadVBuffer)))
		return false;

	UINT i[6] =
	{
		0, 1, 2,
		1, 3, 2
	};
	data.pSysMem = i;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(UINT) * 6;
	desc.StructureByteStride = (sizeof(UINT));
	if (FAILED(hr = device->CreateBuffer(&desc, &data, &this->quadIBuffer)))
		return false;

	return true;
}



