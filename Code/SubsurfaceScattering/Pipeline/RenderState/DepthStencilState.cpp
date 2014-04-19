#include "DepthStencilState.h"
using namespace ShaderStates;

namespace
{
	static D3D11_DEPTH_STENCIL_DESC depthStencilStateDisabledDepthDesc()
	{
			D3D11_DEPTH_STENCIL_DESC ds;
			ZeroMemory(&ds, sizeof(D3D11_DEPTH_STENCIL_DESC));

			ds.DepthEnable							= false;
			ds.DepthFunc							= D3D11_COMPARISON_LESS_EQUAL;
			ds.DepthWriteMask						= D3D11_DEPTH_WRITE_MASK_ZERO;
			ds.StencilEnable						= false;
			ds.StencilReadMask						= D3D11_DEFAULT_STENCIL_READ_MASK;
			ds.StencilWriteMask						= D3D11_DEFAULT_STENCIL_WRITE_MASK;
			ds.FrontFace.StencilDepthFailOp			= D3D11_STENCIL_OP_KEEP;
			ds.FrontFace.StencilFailOp				= D3D11_STENCIL_OP_KEEP;
			ds.FrontFace.StencilPassOp				= D3D11_STENCIL_OP_REPLACE;
			ds.FrontFace.StencilFunc				= D3D11_COMPARISON_ALWAYS;
			ds.BackFace								= ds.FrontFace;

			return ds;
	}
	static D3D11_DEPTH_STENCIL_DESC depthStencilStateEnabledDepthDesc()
	{
			D3D11_DEPTH_STENCIL_DESC ds;
			ZeroMemory(&ds, sizeof(D3D11_DEPTH_STENCIL_DESC));

			ds.DepthEnable                          = true;
			ds.DepthFunc                            = D3D11_COMPARISON_LESS_EQUAL;
			ds.DepthWriteMask                       = D3D11_DEPTH_WRITE_MASK_ALL;
			ds.StencilEnable                        = false;
			ds.StencilReadMask                      = D3D11_DEFAULT_STENCIL_READ_MASK;
			ds.StencilWriteMask						= D3D11_DEFAULT_STENCIL_WRITE_MASK;
			ds.FrontFace.StencilDepthFailOp			= D3D11_STENCIL_OP_KEEP;
			ds.FrontFace.StencilFailOp				= D3D11_STENCIL_OP_KEEP;
			ds.FrontFace.StencilPassOp				= D3D11_STENCIL_OP_REPLACE;
			ds.FrontFace.StencilFunc				= D3D11_COMPARISON_ALWAYS;
			ds.BackFace                             = ds.FrontFace;

			return ds;
	}

	static ID3D11DepthStencilState* depthStencilStateDisabledDepth = NULL;
	static ID3D11DepthStencilState* depthStencilStateEnabledDepth = NULL;
}



void DepthStencilState::Release()
{
	if (depthStencilStateDisabledDepth) depthStencilStateDisabledDepth->Release(); depthStencilStateDisabledDepth = 0;
	if (depthStencilStateEnabledDepth) depthStencilStateEnabledDepth->Release(); depthStencilStateEnabledDepth = 0;
}

ID3D11DepthStencilState* DepthStencilState::GetDisabledDepth(ID3D11Device* device)
{
	if (!depthStencilStateDisabledDepth && device && FAILED(device->CreateDepthStencilState(&depthStencilStateDisabledDepthDesc(), &depthStencilStateDisabledDepth)))
		return false;

	return depthStencilStateDisabledDepth;
}
ID3D11DepthStencilState* DepthStencilState::GetEnabledDepth(ID3D11Device* device)
{
	if (!depthStencilStateEnabledDepth && device && FAILED(device->CreateDepthStencilState(&depthStencilStateEnabledDepthDesc(), &depthStencilStateEnabledDepth)))
		return false;

	return depthStencilStateEnabledDepth;
}