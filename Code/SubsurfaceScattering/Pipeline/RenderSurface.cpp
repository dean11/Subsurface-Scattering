#include "RenderSurface.h"

RenderSurface::RenderSurface()
{
	this->ownRenderTargetSRV = true;
	this->ownDepthStencilSRV = true;
	this->_colorMapSRV = 0;
	this->_colorMapRTV = 0;
	this->_depthMapSRV = 0;
	this->_depthMapDSV = 0;
	this->_width = 0;
	this->_height = 0;
}
RenderSurface::~RenderSurface()
{

}

bool RenderSurface::Create(const TEXTURESURFACE_DESC&  desc, ID3D11Device* device)
{
	this->_width = desc.width;
	this->_height = desc.height;

	if (desc.useColor)
		if (!_CreateSRV(desc, device))
			return false;

	if (desc.useDepth)
		if (!_CreateDSV(desc, device))
			return false;

	return true;
}
void RenderSurface::Release()
{
	if (this->ownRenderTargetSRV && this->_colorMapSRV) this->_colorMapSRV->Release(); this->_colorMapSRV = 0;
	if (this->ownDepthStencilSRV && this->_depthMapSRV) this->_depthMapSRV->Release(); this->_depthMapSRV = 0;
	if (this->_colorMapRTV) this->_colorMapRTV->Release(); this->_colorMapRTV = 0;
	if (this->_depthMapDSV) this->_depthMapDSV->Release(); this->_depthMapDSV = 0;

	this->_width = 0;
	this->_height = 0;
}

ID3D11ShaderResourceView* RenderSurface::GetRenderTargetSRV()
{
	return this->_colorMapSRV;
}
ID3D11ShaderResourceView* RenderSurface::GetDepthStencilSRV()
{
	return this->_depthMapSRV;
}
ID3D11RenderTargetView*	RenderSurface::GetRenderTargetView()
{
	return this->_colorMapRTV;
}
ID3D11DepthStencilView*	RenderSurface::GetDepthStencilView()
{
	return this->_depthMapDSV;
}
						  
void RenderSurface::SetRenderTargetSRV(ID3D11ShaderResourceView* srv, bool transferOwnership)
{
	this->ownRenderTargetSRV = transferOwnership;
	this->_colorMapSRV = srv;
}
void RenderSurface::SetDepthStencilSRV(ID3D11ShaderResourceView* srv, bool transferOwnership)
{
	this->ownDepthStencilSRV = transferOwnership;
	this->_depthMapSRV = srv;
}

//OPERATORS
RenderSurface::operator ID3D11RenderTargetView*()
{
	return this->_colorMapRTV;
}
RenderSurface::operator ID3D11DepthStencilView*()
{
	return this->_depthMapDSV;
}

//PRIVATE
bool RenderSurface::_CreateDSV(const TEXTURESURFACE_DESC& d, ID3D11Device* device)
{
	ID3D11Texture2D* tex = 0;

	if (FAILED(device->CreateTexture2D(d.depthDesc.dsvTexDesc, d.depthDesc.dsvTexSubData, &tex)))
		return false;

	if (FAILED(device->CreateDepthStencilView(tex, d.depthDesc.dsvDesc, &this->_depthMapDSV)))
		return false;

	if (FAILED(device->CreateShaderResourceView(tex, d.depthDesc.dsvSrvDesc, &this->_depthMapSRV)))
		return false;

	tex->Release();

	return true;
}
bool RenderSurface::_CreateSRV(const TEXTURESURFACE_DESC& d, ID3D11Device* device)
{
	ID3D11Texture2D* tex = 0;

	if (FAILED(device->CreateTexture2D(d.colorDesc.texDesc, d.colorDesc.texSubData, &tex)))
		return false;

	if (FAILED(device->CreateRenderTargetView(tex, d.colorDesc.rtvDesc, &this->_colorMapRTV)))
		return false;

	if (FAILED(device->CreateShaderResourceView(tex, d.colorDesc.srvDesc, &this->_colorMapSRV)))
		return false;

	tex->Release();

	return true;
}






