#ifndef BATCHELOR_RENDERTARGET_H
#define BATCHELOR_RENDERTARGET_H

#include <d3d11_2.h>

class RenderSurface
{
public:
	struct DEPTH_DESC
	{
		D3D11_TEXTURE2D_DESC				*dsvTexDesc;	// DEFAULT: 0
		D3D11_SUBRESOURCE_DATA				*dsvTexSubData; // DEFAULT: 0
		D3D11_DEPTH_STENCIL_VIEW_DESC		*dsvDesc;		// DEFAULT: 0
		D3D11_SHADER_RESOURCE_VIEW_DESC		*dsvSrvDesc;	// DEFAULT: 0
		DEPTH_DESC()
			:dsvTexDesc(0)
			, dsvDesc(0)
			, dsvSrvDesc(0)
			, dsvTexSubData(0)
		{}
	};
	struct COLOR_DESC
	{
		D3D11_TEXTURE2D_DESC				*texDesc;		// DEFAULT: 0
		D3D11_SUBRESOURCE_DATA				*texSubData;	// DEFAULT: 0
		D3D11_RENDER_TARGET_VIEW_DESC		*rtvDesc;		// DEFAULT: 0
		D3D11_SHADER_RESOURCE_VIEW_DESC		*srvDesc;		// DEFAULT: 0
		COLOR_DESC()
			:texDesc(0)
			, rtvDesc(0)
			, srvDesc(0)
			, texSubData(0)
		{}
	};
	struct TEXTURESURFACE_DESC
	{
		UINT width;
		UINT height;
		bool useDepth;
		bool useColor;
		DEPTH_DESC depthDesc;
		COLOR_DESC colorDesc;

		TEXTURESURFACE_DESC()
			: useDepth(0)
			, useColor(0)
			, width(0)
			, height(0)
		{}
	};

public:
	RenderSurface();
	virtual~RenderSurface();

	bool						Create(const TEXTURESURFACE_DESC&  desc);
	void						Release();

	ID3D11ShaderResourceView*	GetRenderTargetSRV();
	ID3D11ShaderResourceView*	GetDepthStencilSRV();
	ID3D11RenderTargetView*		GetRenderTargetView();
	ID3D11DepthStencilView*		GetDepthStencilView();

	void						SetRenderTargetSRV(ID3D11ShaderResourceView* srv, bool transferOwnership);
	void						SetDepthStencilSRV(ID3D11ShaderResourceView* srv, bool transferOwnership);

	//OPERATORS
	operator ID3D11RenderTargetView*();
	operator ID3D11DepthStencilView*();

private:
	ID3D11ShaderResourceView	*_colorMapSRV;
	ID3D11RenderTargetView*		_colorMapRTV;

	ID3D11ShaderResourceView	*_depthMapSRV;
	ID3D11DepthStencilView*		_depthMapDSV;

	UINT _width;
	UINT _height;

	bool ownRenderTargetSRV;
	bool ownDepthStencilSRV;

	bool _CreateDSV(const TEXTURESURFACE_DESC& d);
	bool _CreateSRV(const TEXTURESURFACE_DESC& d);

};

#endif // !BATCHELOR_RENDERTARGET_H
