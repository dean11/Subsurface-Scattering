#ifndef SAMPLER_STATE_H
#define SAMPLER_STATE_H

#include <d3d11_2.h>

namespace ShaderStates
{
	class SamplerState
	{
		public:
			static void Release();
			static ID3D11SamplerState* GetPoint(ID3D11Device* device = 0);
			static ID3D11SamplerState* GetLinear(ID3D11Device* device = 0);
			static ID3D11SamplerState* GetAnisotropic2(ID3D11Device* device = 0);
			static ID3D11SamplerState* GetAnisotropic4(ID3D11Device* device = 0);
			static ID3D11SamplerState* GetAnisotropic8(ID3D11Device* device = 0);
			static ID3D11SamplerState* GetAnisotropic16(ID3D11Device* device = 0);


	};

}

#endif

