#ifndef RASTERIZER_STATE_H
#define RASTERIZER_STATE_H

#include <d3d11_2.h>

namespace ShaderStates
{
	class RasterizerState
	{
		public:
			static void Release();
			static ID3D11RasterizerState* GetBackCullNoMS(ID3D11Device* device = 0);
			static ID3D11RasterizerState* GetFrontCullNoMS(ID3D11Device* device = 0);
			static ID3D11RasterizerState* GetNoCullNoMs(ID3D11Device* device = 0);
			static ID3D11RasterizerState* GetNoCullNoMSWF(ID3D11Device* device = 0);
	};
}


#endif