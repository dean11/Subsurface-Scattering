#include "SamplerState.h"
using namespace ShaderStates;

namespace
{
	static D3D11_SAMPLER_DESC samplerStatePointDesc()
	{
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter           = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.AddressU         = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV         = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW         = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias		= 0.0f;
		sd.MaxAnisotropy	= 1;
		sd.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
		sd.BorderColor[0]	= 0.0f;
		sd.BorderColor[1]	= 0.0f;
		sd.BorderColor[2]	= 0.0f;
		sd.BorderColor[3]	= 0.0f;
		sd.MinLOD           = 0.0f;
		sd.MaxLOD           = D3D11_FLOAT32_MAX;
		
		return sd;
	}
	static D3D11_SAMPLER_DESC samplerStateLinearDesc()
	{
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias		= 0.0f;
		sd.MaxAnisotropy	= 1;
		sd.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
		sd.BorderColor[0]	= 0.0f;
		sd.BorderColor[1]	= 0.0f;
		sd.BorderColor[2]	= 0.0f;
		sd.BorderColor[3]	= 0.0f;
		sd.MinLOD			= 0.0f;
		sd.MaxLOD			= D3D11_FLOAT32_MAX;
		
		return sd;
	}
	static D3D11_SAMPLER_DESC samplerStateAnisotropic2Desc()
	{
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter			= D3D11_FILTER_ANISOTROPIC;
		sd.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias		= 0.0f;
		sd.MaxAnisotropy	= 2;
		sd.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
		sd.BorderColor[0]	= 0.0f;
		sd.BorderColor[1]	= 0.0f;
		sd.BorderColor[2]	= 0.0f;
		sd.BorderColor[3]	= 0.0f;
		sd.MinLOD			= 0.0f;
		sd.MaxLOD			= D3D11_FLOAT32_MAX;
	
		return sd;
	}
	static D3D11_SAMPLER_DESC samplerStateAnisotropic4Desc()
	{
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter           = D3D11_FILTER_ANISOTROPIC;
		sd.AddressU         = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV         = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW         = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias		= 0.0f;
		sd.MaxAnisotropy	= 4;
		sd.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
		sd.BorderColor[0]	= 0.0f;
		sd.BorderColor[1]	= 0.0f;
		sd.BorderColor[2]	= 0.0f;
		sd.BorderColor[3]	= 0.0f;
		sd.MinLOD           = 0.0f;
		sd.MaxLOD           = D3D11_FLOAT32_MAX;
	
		return sd;
	}
	static D3D11_SAMPLER_DESC samplerStateAnisotropic8Desc()
	{
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter			= D3D11_FILTER_ANISOTROPIC;
		sd.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias		= 0.0f;
		sd.MaxAnisotropy	= 8;
		sd.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
		sd.BorderColor[0]	= 0.0f;
		sd.BorderColor[1]	= 0.0f;
		sd.BorderColor[2]	= 0.0f;
		sd.BorderColor[3]	= 0.0f;
		sd.MinLOD			= 0.0f;
		sd.MaxLOD			= D3D11_FLOAT32_MAX;

		return sd;
	}
	static D3D11_SAMPLER_DESC samplerStateAnisotropic16Desc()
	{
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter			= D3D11_FILTER_ANISOTROPIC;
		sd.AddressU			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW			= D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias		= 0.0f;
		sd.MaxAnisotropy	= 16;
		sd.ComparisonFunc	= D3D11_COMPARISON_ALWAYS;
		sd.BorderColor[0]	= 0.0f;
		sd.BorderColor[1]	= 0.0f;
		sd.BorderColor[2]	= 0.0f;
		sd.BorderColor[3]	= 0.0f;
		sd.MinLOD			= 0.0f;
		sd.MaxLOD			= D3D11_FLOAT32_MAX;

		return sd;
	}
	static D3D11_SAMPLER_DESC samplerStateShadowMapDesc()
	{
		D3D11_SAMPLER_DESC sd;
		
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter           = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		sd.AddressU         = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV         = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW         = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.MipLODBias		= 0.0f;
		sd.MaxAnisotropy	= 1;
		sd.ComparisonFunc	= D3D11_COMPARISON_LESS;
		sd.BorderColor[0]	= 0.0f;
		sd.BorderColor[1]	= 0.0f;
		sd.BorderColor[2]	= 0.0f;
		sd.BorderColor[3]	= 0.0f;
		sd.MinLOD           = 0.0f;
		sd.MaxLOD           = D3D11_FLOAT32_MAX;
		
		return sd;
	}

    static ID3D11SamplerState* samplerStatePoint;
    static ID3D11SamplerState* samplerStateLinear;
    static ID3D11SamplerState* samplerStateAnisotropic2;
    static ID3D11SamplerState* samplerStateAnisotropic4;
    static ID3D11SamplerState* samplerStateAnisotropic8;
    static ID3D11SamplerState* samplerStateAnisotropic16;
    static ID3D11SamplerState* samplerStateShadowMap;
}

void SamplerState::Release()
{
	if (samplerStatePoint)			samplerStatePoint->Release();			samplerStatePoint = 0;
	if (samplerStateLinear)			samplerStateLinear->Release();			samplerStateLinear = 0;
	if (samplerStateAnisotropic2)	samplerStateAnisotropic2->Release();	samplerStateAnisotropic2 = 0;
	if (samplerStateAnisotropic4)	samplerStateAnisotropic4->Release();	samplerStateAnisotropic4 = 0;
	if (samplerStateAnisotropic8)	samplerStateAnisotropic8->Release();	samplerStateAnisotropic8 = 0;
	if (samplerStateAnisotropic16)	samplerStateAnisotropic16->Release();	samplerStateAnisotropic16 = 0;
	if (samplerStateShadowMap)		samplerStateShadowMap->Release();		samplerStateShadowMap = 0;
}
ID3D11SamplerState* SamplerState::GetPoint(ID3D11Device* device)
{
	if (!samplerStatePoint && device && FAILED(device->CreateSamplerState(&samplerStatePointDesc(), &samplerStatePoint)))
		return false;
	
	return samplerStatePoint;
}
ID3D11SamplerState* SamplerState::GetLinear(ID3D11Device* device)
{
	if (!samplerStateLinear && device && FAILED(device->CreateSamplerState(&samplerStateLinearDesc(), &samplerStateLinear)))
		return false;
	
	return samplerStateLinear;
}
ID3D11SamplerState* SamplerState::GetAnisotropic2(ID3D11Device* device)
{
	if (!samplerStateAnisotropic2 && device && FAILED(device->CreateSamplerState(&samplerStateAnisotropic2Desc(), &samplerStateAnisotropic2)))
		return false;

	return samplerStateAnisotropic2;
}
ID3D11SamplerState* SamplerState::GetAnisotropic4(ID3D11Device* device)
{
	if (!samplerStateAnisotropic4 && device && FAILED(device->CreateSamplerState(&samplerStateAnisotropic4Desc(), &samplerStateAnisotropic4)))
		return false;
	
	return samplerStateAnisotropic4;
}
ID3D11SamplerState* SamplerState::GetAnisotropic8(ID3D11Device* device)
{
	if (!samplerStateAnisotropic8 && device && FAILED(device->CreateSamplerState(&samplerStateAnisotropic8Desc(), &samplerStateAnisotropic8)))
		return false;
	
	return samplerStateAnisotropic8;
}
ID3D11SamplerState* SamplerState::GetAnisotropic16(ID3D11Device* device)
{
	if (!samplerStateAnisotropic16 && device && FAILED(device->CreateSamplerState(&samplerStateAnisotropic16Desc(), &samplerStateAnisotropic16)))
		return false;

	return samplerStateAnisotropic16;
}
ID3D11SamplerState* SamplerState::GetShadow(ID3D11Device* device)
{
	if (!samplerStateShadowMap && device && FAILED(device->CreateSamplerState(&samplerStateShadowMapDesc(), &samplerStateShadowMap)))
		return false;

	return samplerStateShadowMap;
}



