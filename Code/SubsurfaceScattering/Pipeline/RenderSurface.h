#ifndef BATCHELOR_RENDERTARGET_H
#define BATCHELOR_RENDERTARGET_H

#include <d3d11_2.h>

class RenderSurface
{
public:
	struct DEPTHSTENCIL_SRV_DESC
	{
		D3D11_TEXTURE2D_DESC				*dsvTexDesc;
		D3D11_SUBRESOURCE_DATA				*dsvTexSubData;
		D3D11_DEPTH_STENCIL_VIEW_DESC		*dsvDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC		*dsvSrvDesc;
	};
	struct RENDERTARGET_SRV_DESC
	{
		D3D11_TEXTURE2D_DESC				*texDesc;
		D3D11_SUBRESOURCE_DATA				*texSubData;
		D3D11_RENDER_TARGET_VIEW_DESC		*rtvDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC		*srvDesc;
	};
	struct TEXTURESURFACE_DESC
	{
		UINT width;
		UINT height;
		bool useDepth;
		bool useColor;
		DEPTHSTENCIL_SRV_DESC depthDesc;
		RENDERTARGET_SRV_DESC colorDesc;
	};

public:
	RenderSurface();
	virtual~RenderSurface();

	bool						Create(const TEXTURESURFACE_DESC&  desc, ID3D11Device* device);
	void						Release();

	ID3D11ShaderResourceView*	GetRenderTargetSRV();
	ID3D11ShaderResourceView*	GetDepthStencilSRV();
	ID3D11RenderTargetView*		GetRenderTargetView();
	ID3D11DepthStencilView*		GetDepthStencilView();

	void						SetRenderTargetSRV(ID3D11ShaderResourceView* srv, bool transferOwnership = false);
	void						SetDepthStencilSRV(ID3D11ShaderResourceView* srv, bool transferOwnership = false);

	//OPERATORS
	operator ID3D11RenderTargetView*();
	operator ID3D11DepthStencilView*();

private:
	bool _CreateDSV(const TEXTURESURFACE_DESC& d, ID3D11Device* device);
	bool _CreateSRV(const TEXTURESURFACE_DESC& d, ID3D11Device* device);

private:
	ID3D11ShaderResourceView	*_colorMapSRV;
	ID3D11RenderTargetView		*_colorMapRTV;

	ID3D11ShaderResourceView	*_depthMapSRV;
	ID3D11DepthStencilView		*_depthMapDSV;

	UINT _width;
	UINT _height;

	bool ownRenderTargetSRV;
	bool ownDepthStencilSRV;
};

#endif // !BATCHELOR_RENDERTARGET_H
