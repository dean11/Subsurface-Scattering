#ifndef DEPTH_STENCIL_STATES_H
#define DEPTH_STENCIL_STATES_H

#include <d3d11_2.h>

namespace ShaderStates
{
	class DepthStencilState
	{
		public:
			static void Release();
	        static ID3D11DepthStencilState* GetDisabledDepth(ID3D11Device* device = 0);
			static ID3D11DepthStencilState* GetEnabledDepth(ID3D11Device* device = 0);
	};
}

#endif