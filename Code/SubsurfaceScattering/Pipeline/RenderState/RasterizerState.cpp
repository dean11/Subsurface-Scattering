#include "RasterizerState.h"
using namespace ShaderStates;

namespace
{
	static D3D11_RASTERIZER_DESC rasterizerStateBackCullNoMSDesc()
	{
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory(&rd, sizeof(rd));
        rd.AntialiasedLineEnable		= false;
        rd.CullMode						= D3D11_CULL_BACK;
        rd.DepthBias					= 0;
        rd.DepthBiasClamp				= 0.0f;
        rd.DepthClipEnable				= true;
        rd.FillMode						= D3D11_FILL_SOLID;
        rd.FrontCounterClockwise		= false;
        rd.MultisampleEnable			= false;
        rd.ScissorEnable				= false;
        rd.SlopeScaledDepthBias			= 0.0f;

        return rd;
	}
	static D3D11_RASTERIZER_DESC rasterizerStateFrontCullNoMSDesc()
	{
        D3D11_RASTERIZER_DESC rd;
        ZeroMemory(&rd, sizeof(rd));
        rd.AntialiasedLineEnable		= false;
        rd.CullMode						= D3D11_CULL_FRONT;
        rd.DepthBias					= 0;
        rd.DepthBiasClamp				= 0.0f;
        rd.DepthClipEnable				= true;
        rd.FillMode						= D3D11_FILL_SOLID;
        rd.FrontCounterClockwise		= false;
        rd.MultisampleEnable			= false;
        rd.ScissorEnable				= false;
        rd.SlopeScaledDepthBias			= 0.0f;

        return rd;
	}
	static D3D11_RASTERIZER_DESC rasterizerStateNoCullNoMsDesc()
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.AntialiasedLineEnable		= false;
		rd.CullMode						= D3D11_CULL_NONE;
		rd.DepthBias					= 0;
		rd.DepthBiasClamp				= 0.0f;
		rd.DepthClipEnable				= true;
		rd.FillMode						= D3D11_FILL_SOLID;
		rd.FrontCounterClockwise		= false;
		rd.MultisampleEnable			= false;
		rd.ScissorEnable				= false;
		rd.SlopeScaledDepthBias			= 0.0f;
	
		return rd;
	}
	static D3D11_RASTERIZER_DESC rasterizerStateNoCullNoMSWFDesc()
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.AntialiasedLineEnable		= false;
		rd.CullMode						= D3D11_CULL_NONE;
		rd.DepthBias					= 0;
		rd.DepthBiasClamp				= 0.0f;
		rd.DepthClipEnable				= true;
		rd.FillMode						= D3D11_FILL_WIREFRAME;
		rd.FrontCounterClockwise		= false;
		rd.MultisampleEnable			= false;
		rd.ScissorEnable				= false;
		rd.SlopeScaledDepthBias			= 0.0f;
	
		return rd;
	}

	static ID3D11RasterizerState* rasterizerStateBackCull_NoMS = NULL;
	static ID3D11RasterizerState* rasterizerStateFrontCull_NoMS = NULL;
	static ID3D11RasterizerState* rasterizerStateNoCull_NoMs = NULL;
	static ID3D11RasterizerState* rasterizerStateNoCull_NoMS_WF = NULL;
	static int RasterizerStateUseCount = 0;	//Count number if users for safe delete
}




void RasterizerState::Release()
{
	if (rasterizerStateBackCull_NoMS)	rasterizerStateBackCull_NoMS->Release();	rasterizerStateBackCull_NoMS = NULL;
	if (rasterizerStateFrontCull_NoMS)	rasterizerStateFrontCull_NoMS->Release();	rasterizerStateFrontCull_NoMS = NULL;
	if (rasterizerStateNoCull_NoMs)		rasterizerStateNoCull_NoMs->Release();		rasterizerStateNoCull_NoMs = NULL;
	if (rasterizerStateNoCull_NoMS_WF)	rasterizerStateNoCull_NoMS_WF->Release();	rasterizerStateNoCull_NoMS_WF = NULL;
}
ID3D11RasterizerState* RasterizerState::GetBackCullNoMS(ID3D11Device* device)
{
	if (!rasterizerStateBackCull_NoMS && device && FAILED(device->CreateRasterizerState(&rasterizerStateBackCullNoMSDesc(), &rasterizerStateBackCull_NoMS)))
		return false;
	return rasterizerStateBackCull_NoMS;
}
ID3D11RasterizerState* RasterizerState::GetFrontCullNoMS(ID3D11Device* device)
{
	if (!rasterizerStateFrontCull_NoMS && device && FAILED(device->CreateRasterizerState(&rasterizerStateFrontCullNoMSDesc(), &rasterizerStateFrontCull_NoMS)))
		return false;
	return rasterizerStateFrontCull_NoMS;
}
ID3D11RasterizerState* RasterizerState::GetNoCullNoMs(ID3D11Device* device)
{
	if (!rasterizerStateNoCull_NoMs && device && FAILED(device->CreateRasterizerState(&rasterizerStateNoCullNoMsDesc(), &rasterizerStateNoCull_NoMs)))
		return false;
	return rasterizerStateNoCull_NoMs;
}
ID3D11RasterizerState* RasterizerState::GetNoCullNoMSWF(ID3D11Device* device)
{
	if (!rasterizerStateNoCull_NoMS_WF && device && FAILED(device->CreateRasterizerState(&rasterizerStateNoCullNoMSWFDesc(), &rasterizerStateNoCull_NoMS_WF)))
		return false;
	return rasterizerStateNoCull_NoMS_WF;
}


