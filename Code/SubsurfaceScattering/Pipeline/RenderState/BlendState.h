#ifndef BLEND_STATE_H
#define BLEND_STATE_H

#include <d3d11_2.h>

namespace ShaderStates
{
	class BlendStates
	{
		public:
			static void Release();
			static ID3D11BlendState* GetAlphaBlend(ID3D11Device* device = 0);
			static ID3D11BlendState* GetDisabledBlend(ID3D11Device* device = 0);
	};
}

#endif