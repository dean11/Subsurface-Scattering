#include "SSSPass.h"
#include "..\..\Utilities\Util.h"

struct SSSData
{
	SimpleMath::Matrix LightView;
	SimpleMath::Matrix LightProjection;
	SimpleMath::Vector3 LightDirection;

	float pad[1];
};

using namespace Pipeline;

SSSPass::SSSPass()
{

}
SSSPass::~SSSPass()
{

}
void SSSPass::Release()
{
	Util::SAFE_RELEASE(this->frameBuffer);
	this->sssShader.Release();
}
void SSSPass::Clear()
{

}
void SSSPass::Apply(ID3D11ShaderResourceView* srvWorldPositionMap, ID3D11ShaderResourceView* srvNomalMap, const ShadowMap* depths, unsigned int totalDepths)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	if( SUCCEEDED( this->deviceContext->Map(this->frameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr)))
	{
		SSSData *d = (SSSData*)msr.pData;
		
		this->deviceContext->Unmap(this->frameBuffer, 0);
	}
	this->deviceContext->Dispatch((unsigned int)((this->width + 31) / 32), (unsigned int)((this->height + 31) / 32), 1);
}
bool SSSPass::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	this->device = device;
	this->deviceContext = deviceContext;
	this->width	= width;
	this->height = height;

	//if(!this->sssShader.LoadCompiledShader("Shaders\\SubsurfaceScattering\\SSS.compute.cso", Pipeline::ShaderType_CS, device, deviceContext))
	//	return false;

	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = sizeof(SSSData);
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = sizeof(SSSData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	if( FAILED(device->CreateBuffer(&bd, 0, &this->frameBuffer)))
		return false;

	return true;
}
