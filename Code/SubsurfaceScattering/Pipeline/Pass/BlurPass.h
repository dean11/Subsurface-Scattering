#ifndef GUARD_BLURPASS_H
#define GUARD_BLURPASS_H

#include <D3DTK\SimpleMath.h>
#include <d3d11.h>
#include <vector>

#include "..\Shader.h"
#include "..\RenderSurface.h"
#include "ShaderPass.h"
#include "..\..\Utilities\DynamicArray.h"

using namespace Pipeline;

class BlurPass		:public ShaderPass
{
public:
	BlurPass();
	virtual~BlurPass();

	void Apply(ID3D11ShaderResourceView* src, ID3D11UnorderedAccessView* dest, ID3D11ShaderResourceView* translucencyTexture, ID3D11ShaderResourceView* depthMap);
	void Release() override;
	void Clear() override;
	bool Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height);
	bool Recompile();

	void SetKernel(const DirectX::SimpleMath::Vector4* kernel, int kernelSize);
	//void SetKernel(const float* kernel, int kernelSize);
	void SetSSSStrength(float strength);

private:
	struct BlurData
	{
		float sssStrength;

		float pad[3];
	};
	enum ShaderTextureRegisterLayout
	{
		ShaderTextureRegisterLayout_TranslucentTexture = 0,
		ShaderTextureRegisterLayout_DepthMap  = 1,
		ShaderTextureRegisterLayout_SampleKernel  = 2,
	};
	enum ShaderUAVRegisterLayout
	{
		ShaderUAVRegisterLayout_BlurOutData  = 0,
	};
	enum ShaderBufferRegisterLayout
	{
		ShaderBufferRegisterLayout_BlurData = 0,
	};
	enum ShaderSamplerRegisterLayout
	{
		ShaderSamplerRegisterLayout_PointSampler = 0,
	};

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	ID3D11UnorderedAccessView* tempBlurOutputUAV;
	ID3D11ShaderResourceView* tempBlurOutputSRV;
	
	std::vector<DirectX::SimpleMath::Vector4> kernel;
	//std::vector<float> kernel;
	BlurData blurData;

	Shader horizontalBlur;
	Shader verticalBlur;

	ID3D11Buffer* frameBuffer;

	ID3D11Buffer* kenrnelBuffer;
	ID3D11ShaderResourceView* kenrnelBufferSRV;

	DirectX::SimpleMath::Vector2 size;
	bool kernelModified;
	int computeThreads;
	int blurRadius;

	bool CompileSSSShaders();
};

#endif // !GUARD_BLURPASS_H
