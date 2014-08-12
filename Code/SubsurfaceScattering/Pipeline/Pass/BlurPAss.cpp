#include "BlurPass.h"

#include <d3dcompiler.h>
#include <D3DTK\SimpleMath.h>

#include "..\RenderState\DepthStencilState.h"
#include "..\RenderState\RasterizerState.h"
#include "..\RenderState\SamplerState.h"
#include "..\..\Utilities\Util.h"
#include "..\..\Input.h"

using namespace DirectX;

bool BlurPass::CompileSSSShaders()
{
	char BLUR_RADIUS[24];
	_itoa_s(blurRadius, BLUR_RADIUS, 10);

	char THREAD_COUNT[24];
	_itoa_s(computeThreads, THREAD_COUNT, 10);

	char CACHE_SIZE[24];
	_itoa_s(computeThreads + (2 * blurRadius), CACHE_SIZE, 10);

	D3D_SHADER_MACRO shaderMacro[] =
	{
		{ "THREAD_COUNT", THREAD_COUNT },
		{ "BLUR_RADIUS", BLUR_RADIUS },
		{ "CACHE_SIZE", CACHE_SIZE },
		{0,0}
	};

	if (!Shader::CompileShaderToCSO("..\\Code\\SubsurfaceScattering\\Shaders\\Blur.header.hlsli", "Shaders\\Blur.header.cso", "fx_5_0", 0, shaderMacro, Pipeline::ShaderType_None, device, deviceContext)) return false;
	if (!this->horizontalBlur.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\BlurHor.compute.hlsl", "cs_5_0", 0, shaderMacro, Pipeline::ShaderType_CS, device, deviceContext)) return false;
	if (!this->verticalBlur.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\BlurVert.compute.hlsl", "cs_5_0", 0, shaderMacro, Pipeline::ShaderType_CS, device, deviceContext)) return false;

	return true;
}


BlurPass::BlurPass()
{
	this->blurRadius = 5;
	this->computeThreads = 256;

	//float tmp[] =
	//{
	//	0.2f,
	//	0.05f,	
	//	0.05f,	
	//	0.1f,	
	//	0.1f,	
	//	0.1f,	
	//	0.1f,	
	//	0.1f,	
	//	0.1f,	
	//	0.05f,	
	//	0.05f,	
	//};
	SimpleMath::Vector4 tmp[] = 
	{
		SimpleMath::Vector4(0.56047900f, 0.669086000f, 0.7847280000f, 0.000f),			//Middle sample
	
		SimpleMath::Vector4(0.00471691f	, 0.000184771f	, 5.07566e-005f	, -2.00f),
		SimpleMath::Vector4(0.01928310f	, 0.002820180f	, 0.0008421400f	, -1.28f),
		SimpleMath::Vector4(0.03639000f	, 0.013099900f	, 0.0064368500f	, -0.72f),
		SimpleMath::Vector4(0.08219040f	, 0.035860800f	, 0.0209261000f	, -0.32f),
		SimpleMath::Vector4(0.07718020f	, 0.113491000f	, 0.0793803000f	, -0.08f),
		
		SimpleMath::Vector4(0.07718020f	, 0.113491000f	, 0.0793803000f	, 0.080f),
		SimpleMath::Vector4(0.08219040f	, 0.035860800f	, 0.0209261000f	, 0.320f),
		SimpleMath::Vector4(0.03639000f	, 0.013099900f	, 0.0064368500f	, 0.720f),
		SimpleMath::Vector4(0.01928310f	, 0.002820180f	, 0.0008421400f	, 1.280f),
		SimpleMath::Vector4(0.00471691f	, 0.000184771f	, 5.07565e-005f	, 2.000f),
	};

	this->kernelModified = true;
	this->kernel.resize(Util::NumElementsOf(tmp));
	memcpy(&this->kernel[0], &tmp[0], sizeof(tmp)); 
	this->blurData.sssStrength = 1.0f;
	this->device = 0;
	this->deviceContext = 0;
	this->frameBuffer = 0;
	this->kenrnelBuffer = 0;
	this->kenrnelBufferSRV = 0;
}
BlurPass::~BlurPass()
{
	this->device = 0;
	this->deviceContext = 0;
	this->frameBuffer = 0;
	this->kenrnelBuffer = 0;
	this->kenrnelBufferSRV = 0;
}

void BlurPass::Apply(ID3D11ShaderResourceView* src, ID3D11UnorderedAccessView* dest, ID3D11ShaderResourceView* translucencyTexture, ID3D11ShaderResourceView* depthMap)
{
		 if (Input::IsKeyDown(VK_B)) return;
	else if (Input::IsKeyDown(VK_F8)) CompileSSSShaders();

	//Set data
	D3D11_MAPPED_SUBRESOURCE res;
	if(FAILED(this->deviceContext->Map(this->frameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
		return;
	BlurData* resD = (BlurData*)res.pData;
	resD->sssStrength = this->blurData.sssStrength;
	this->deviceContext->Unmap(this->frameBuffer, 0);

	if (this->kernelModified)
	{
		if (FAILED(this->deviceContext->Map(this->kenrnelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
			return;
		SimpleMath::Vector4* data = (SimpleMath::Vector4*)res.pData;
		memcpy(&data[0], &this->kernel[0], sizeof(SimpleMath::Vector4) * this->kernel.size());
		this->deviceContext->Unmap(this->kenrnelBuffer, 0);
		this->kernelModified = false;
	}
	//if (this->kernelModified)
	//{
	//	if (FAILED(this->deviceContext->Map(this->kenrnelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
	//		return;
	//	float* data = (float*)res.pData;
	//	memcpy(&data[0], &this->kernel[0], sizeof(float) * this->kernel.size());
	//	this->deviceContext->Unmap(this->kenrnelBuffer, 0);
	//	this->kernelModified = false;
	//}

	this->deviceContext->CSSetConstantBuffers(0, 1, &this->frameBuffer);

	ID3D11ShaderResourceView* srv[] = { src, translucencyTexture, depthMap, this->kenrnelBufferSRV };
	this->deviceContext->CSSetShaderResources(0, Util::NumElementsOf(srv), srv);

	//First we render to a temporary uav.
	this->deviceContext->CSSetUnorderedAccessViews(0, 1, &this->tempBlurOutputUAV, 0);

	ID3D11SamplerState* smp[] = { ShaderStates::SamplerState::GetPoint(), ShaderStates::SamplerState::GetLinear() };
	this->deviceContext->CSSetSamplers(0, Util::NumElementsOf(smp), smp);

	//Start with horizontal pass
	this->horizontalBlur.Apply();
	UINT a = (UINT)((this->size.x + ((UINT)this->size.x % this->computeThreads)) / this->computeThreads);
	this->deviceContext->Dispatch( a, (UINT)this->size.y, 1);

	//Set uav register to 0 and aviod warnings.
	ID3D11UnorderedAccessView* tmp[1] = {0};
	this->deviceContext->CSSetUnorderedAccessViews(0, 1, tmp, 0);

	this->verticalBlur.Apply();
	ID3D11ShaderResourceView* rv[] = { this->tempBlurOutputSRV };
	this->deviceContext->CSSetShaderResources(0, Util::NumElementsOf(rv), rv);
	this->deviceContext->CSSetUnorderedAccessViews(0, 1, &dest, 0);
	a = (UINT)((this->size.y + ((UINT)this->size.y % this->computeThreads)) / this->computeThreads);
	this->deviceContext->Dispatch((UINT)this->size.x, a, 1);
}
void BlurPass::Release()
{
	this->device = 0;
	this->deviceContext = 0;

	Util::SAFE_RELEASE(this->frameBuffer);
	Util::SAFE_RELEASE(this->kenrnelBuffer);
	Util::SAFE_RELEASE(this->kenrnelBufferSRV);
	Util::SAFE_RELEASE(this->tempBlurOutputSRV);
	Util::SAFE_RELEASE(this->tempBlurOutputUAV);

	horizontalBlur.Release();
	verticalBlur.Release();
}
void BlurPass::Clear()
{
	ID3D11Buffer* buff[1] = {0};
	this->deviceContext->CSSetConstantBuffers(0, 1, buff);

	ID3D11ShaderResourceView* srv[3] = {0};
	this->deviceContext->CSSetShaderResources(0, 3, srv);

	ID3D11UnorderedAccessView* uav[1] = {0};
	this->deviceContext->CSSetUnorderedAccessViews(0, 1, uav, 0);

	ID3D11SamplerState* smp[1] = {0};
	this->deviceContext->CSSetSamplers(0, 1, smp);
}

bool BlurPass::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	this->size = SimpleMath::Vector2((float)width, (float)height);
	this->device = device;
	this->deviceContext = deviceContext;

	if (!CompileSSSShaders())
		return false;

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = sizeof(BlurData);
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = sizeof(BlurData);
	bd.Usage = D3D11_USAGE_DYNAMIC;
	if( FAILED ( this->device->CreateBuffer(&bd, 0, &this->frameBuffer)) )
		return false;

	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//cbd.ByteWidth = ((sizeof(float)) * this->kernel.size());
	cbd.ByteWidth = ((sizeof(float)* 4) * this->kernel.size());
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//cbd.StructureByteStride = (sizeof(float));
	cbd.StructureByteStride = (sizeof(float)* 4);
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	if (FAILED( this->device->CreateBuffer(&cbd, 0, &this->kenrnelBuffer))) 
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_UNKNOWN;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvd.Buffer.ElementOffset = 0;
	srvd.Buffer.ElementWidth = this->kernel.size();
	if ( FAILED( this->device->CreateShaderResourceView(this->kenrnelBuffer, &srvd, &this->kenrnelBufferSRV)))
		return false;	

	ID3D11Texture2D *tex = 0;
	D3D11_TEXTURE2D_DESC t2d = {0};
	t2d.ArraySize = 1;
	t2d.MipLevels = 1;
	t2d.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	t2d.CPUAccessFlags = 0;
	t2d.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	t2d.Width = width;
	t2d.Height = height;
	t2d.MiscFlags = 0;
	t2d.SampleDesc.Count = 1;
	t2d.SampleDesc.Quality = 0;
	t2d.Usage = D3D11_USAGE_DEFAULT;
	if( FAILED ( this->device->CreateTexture2D(&t2d, 0, &tex) ) )
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc1;
	srvDesc1.Format = t2d.Format;
	srvDesc1.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc1.Texture2D.MostDetailedMip = 0;
	srvDesc1.Texture2D.MipLevels = 1;
	if (FAILED(this->device->CreateShaderResourceView(tex, &srvDesc1, &this->tempBlurOutputSRV)))
		return false;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Texture2D.MipSlice = 0;
	uavDesc.Format = t2d.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	if ( FAILED ( this->device->CreateUnorderedAccessView(tex, &uavDesc, &this->tempBlurOutputUAV) ) )
		return false;

	tex->Release();

	return true;
}

void BlurPass::SetKernel(const DirectX::SimpleMath::Vector4* kernel, int kernelSize)
//void BlurPass::SetKernel(const float* kernel, int kernelSize)
{
	if (kernelSize != this->kernel.size())
	{
		Util::SAFE_RELEASE(this->kenrnelBuffer);
		
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		cbd.ByteWidth = ((sizeof(float)* 4) * kernelSize);
		//cbd.ByteWidth = ((sizeof(float)) * kernelSize);
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		cbd.StructureByteStride = (sizeof(float)* 4);
		//cbd.StructureByteStride = (sizeof(float));
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		if (FAILED(this->device->CreateBuffer(&cbd, 0, &this->kenrnelBuffer)))
		{
			printf("Failed to update blur kernel!\n");
			return;
		}
	}

	int radius = (kernelSize - 1) / 2;

	this->kernel.clear();
	this->kernel.resize( radius );
	//memcpy(&this->kernel[0], kernel, sizeof(SimpleMath::Vector4) * kernelSize);
	memcpy(&this->kernel[0], kernel, sizeof(float) * kernelSize);

	CompileSSSShaders();

	this->kernelModified = true;
}
void BlurPass::SetSSSStrength(float strength)
{
	this->blurData.sssStrength = strength;
}
