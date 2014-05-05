#include "BlendState.h"
using namespace ShaderStates;

namespace
{
	static D3D11_BLEND_DESC blendStateAlphaBlendDesc()
	{
			D3D11_BLEND_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));
			desc.AlphaToCoverageEnable = true;
			desc.IndependentBlendEnable = false;
			for (unsigned int i = 0; i < 8; ++i)
			{
				desc.RenderTarget[i].BlendEnable			= true;
				desc.RenderTarget[i].BlendOp				= D3D11_BLEND_OP_ADD;
				desc.RenderTarget[i].BlendOpAlpha			= D3D11_BLEND_OP_MAX;
				desc.RenderTarget[i].DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;
				desc.RenderTarget[i].DestBlendAlpha			= D3D11_BLEND_ONE;
				desc.RenderTarget[i].RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;
				desc.RenderTarget[i].SrcBlend				= D3D11_BLEND_SRC_ALPHA;
				desc.RenderTarget[i].SrcBlendAlpha			= D3D11_BLEND_ONE;
				
			}

			return desc;
	}
	static D3D11_BLEND_DESC blendStateDisabledBlendDesc()
	{
			D3D11_BLEND_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.AlphaToCoverageEnable = false;
			bd.IndependentBlendEnable = false;
			for (unsigned int i = 0; i < 8; ++i)
			{
				bd.RenderTarget[i].BlendEnable                  = false;
				bd.RenderTarget[i].BlendOp						= D3D11_BLEND_OP_ADD;
				bd.RenderTarget[i].BlendOpAlpha					= D3D11_BLEND_OP_ADD;
				bd.RenderTarget[i].DestBlend					= D3D11_BLEND_INV_SRC_ALPHA;
				bd.RenderTarget[i].DestBlendAlpha				= D3D11_BLEND_ONE;
				bd.RenderTarget[i].RenderTargetWriteMask		= D3D11_COLOR_WRITE_ENABLE_ALL;
				bd.RenderTarget[i].SrcBlend                     = D3D11_BLEND_SRC_ALPHA;
				bd.RenderTarget[i].SrcBlendAlpha                = D3D11_BLEND_ONE;
			}

			return bd;
	}
		
	static ID3D11BlendState* blendStateAlphaBlend = NULL;
	static ID3D11BlendState* blendStateDisabledBlend = NULL;
}


void BlendStates::Release()
{
	if (blendStateAlphaBlend) blendStateAlphaBlend->Release(); blendStateAlphaBlend = 0;
	if (blendStateDisabledBlend) blendStateDisabledBlend->Release(); blendStateDisabledBlend = 0;
}
ID3D11BlendState* BlendStates::GetAlphaBlend(ID3D11Device* device)
{
	if (!blendStateAlphaBlend && device && FAILED(device->CreateBlendState(&blendStateAlphaBlendDesc(), &blendStateAlphaBlend)))
		return false;

	return blendStateAlphaBlend;
}
ID3D11BlendState* BlendStates::GetDisabledBlend(ID3D11Device* device)
{
	if (!blendStateDisabledBlend && device &&  FAILED(device->CreateBlendState(&blendStateDisabledBlendDesc(), &blendStateDisabledBlend)))
		return false;

	return blendStateDisabledBlend;
}

