#ifndef INCLUDEGUARD_POSTEFFECTPASS_H
#define INCLUDEGUARD_POSTEFFECTPASS_H

#include <d3d11.h>
#include <vector>

#include "..\Shader.h"
#include "..\RenderSurface.h"
#include "..\..\Utilities\BasicLightData.h"
#include "ShaderPass.h"
#include "..\..\Utilities\clock.h"

class PostEffectPass		:public ShaderPass
{
public:
	static const int maxShadowMaps = 5;

public:
	enum SrvRegister
	{
		SrvRegister_Diffuse					= 0,
		SrvRegister_Normal					= 1,
		SrvRegister_Position				= 2,
		SrvRegister_Thickness				= 3,
		SrvRegister_shadowMap				= 4,

		SrvRegister_PointLightBufferSRV		= SrvRegister_shadowMap + maxShadowMaps,
		SrvRegister_SpotLightBufferSRV		,
		SrvRegister_DirLightBufferSRV		,
		SrvRegister_ShadowBufferSRV			,

		SrvRegister_COUNT
	};

public:
	struct PostPassData
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT3 ambientLight;
		DirectX::XMFLOAT3 cameraPos;
		DirectX::XMFLOAT4X4 projection;

		ID3D11ShaderResourceView* normalMap;
		ID3D11ShaderResourceView* diffuseMap;
		ID3D11ShaderResourceView* positionMap;
		ID3D11ShaderResourceView* thicknessMap;

		const BasicLightData::PointLight* pointData;
		int pointCount;

		const BasicLightData::Spotlight* spotData;
		int spotCount;

		const BasicLightData::Directional* dirData;
		int dirCount;

		BasicLightData::ShadowMapLight*const* shadowData;
		int shadowCount;
	};

public:
	PostEffectPass();
	PostEffectPass(unsigned int maxPointLights, unsigned int maxDirectionalLights, unsigned int maxSpotLights);
	virtual~PostEffectPass() { }

	void Release() override;
	void Clear() override;

	bool ReloadPostEffectShaders();

	void Apply(const PostPassData& data);

	bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, IDXGISwapChain* swapChain);

private:
	void UpdateConstantBuffer(const PostPassData& data);
	void UpdateShadowMaps(const PostPassData& data, ID3D11ShaderResourceView* srv[]);
	void UpdateLights(const PostPassData& data);
	
	bool CreateLightBuffer();
	bool CreateShadowBuffer();
	bool CreateBackBufferUAV(IDXGISwapChain* swap);

	unsigned int width;
	unsigned int height;

private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
		
	ID3D11UnorderedAccessView* backBufferUAV;

	ID3D11ShaderResourceView* pointLightBufferSRV;
	ID3D11ShaderResourceView* spotLightBufferSRV;
	ID3D11ShaderResourceView* dirLightBufferSRV;
	ID3D11ShaderResourceView* shadowBufferSRV;

	ID3D11Buffer* frameBuffer;
	ID3D11Buffer* shadowBuffer;
	ID3D11Buffer* lightBuffer;

	Shader postShader;

	int firstPointLight;
	int firstDirLight;
	int firstSpotLight;

	int maxPointLight;
	int maxDirLight;
	int maxSpotLight;

	bool isCleared;
	Utility::WinTimer benchmarkClock;
};

#endif // !INCLUDEGUARD_POSTEFFECTPASS_H
