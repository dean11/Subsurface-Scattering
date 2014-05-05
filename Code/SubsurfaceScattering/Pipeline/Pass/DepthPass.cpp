#include "DepthPass.h"
#include "..\InputLayoutState.h"
#include "..\RenderState\DepthStencilState.h"
#include "..\RenderState\RasterizerState.h"
#include "..\RenderState\SamplerState.h"

using namespace Pipeline;
#include <d3dcompiler.h>

DepthPass::DepthPass()
{

}

DepthPass::~DepthPass()
{

}

void DepthPass::Release()
{
	//this->cubeDSV->Release();
	this->singleDSV->Release();
	//this->depthCube->Release();
	this->depthMap->Release();
}

void DepthPass::Clear()
{
	/*static ID3D11RenderTargetView* rtv[1] = { 0 };
	this->dc->OMSetRenderTargets(1, rtv, 0);
	ClearSingleDSV();*/
}

void DepthPass::Apply(DepthMapType depthMapType)
{
	//if (depthMapType == SingleDepthMap) 
	ApplySingle();
	//else ApplyCube();
	this->dc->RSSetState(ShaderStates::RasterizerState::GetNoCullNoMs());
	this->dc->OMSetDepthStencilState(ShaderStates::DepthStencilState::GetEnabledDepth(), 0);

	/*ID3D11SamplerState* smp[] = { ShaderStates::SamplerState::GetLinear() };
	this->dc->VSSetSamplers(0, 1, smp);*/
	this->dc->PSSetShader(0, 0, 0);
	this->dc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
}

void DepthPass::Render(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 lookAt)
{
	SetCamera(pos, lookAt);
}
/*
void DepthPass::Render(DirectX::XMFLOAT3 pos, CubeFace face)
{
	switch (face)
	{
	case PositiveX:
		SetCameraForPositiveX(pos);
			break;
	case NegativeX:
		SetCameraForNegativeX(pos);
		break;
	case PositiveY:
		SetCameraForPositiveY(pos);
		break;
	case NegativeY: 
		SetCameraForNegativeY(pos);
		break;
	case PositiveZ:
		SetCameraForPositiveZ(pos);
		break;
	case NegativeZ: 
		SetCameraForNegativeZ(pos);
		break;
	}
}
*/
bool DepthPass::Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile)
{
	this->device = device;
	this->dc = deviceContext;

	posX = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	negX = DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f);
	posY = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	negY = DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f);
	posZ = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	negZ = DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);



	UINT flag = 0;
#if defined(DEBUG) || defined(_DEBUG)
	flag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	bool result = true;
	std::string file;

	if (forceShaderCompile)
	{
		if (!this->depthSingleShader.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\DepthPassSingle.vertex.hlsl", "vs_5_0", flag, 0, ShaderType_VS, device, deviceContext))
			return false;
		/*if (!this->depthCubeShader.CreateShader("..\\Code\\SubsurfaceScattering\\Shaders\\DepthPassCube.vertex.hlsl", "vs_5_0", flag, 0, ShaderType_VS, device, deviceContext))
			return false;*/
	}
	else
	{
		if (!this->depthSingleShader.LoadCompiledShader("Shaders\\DepthPassSingle.vertex.cso", ShaderType_VS, device, deviceContext))
			return false;
		/*if (!this->depthCubeShader.LoadCompiledShader("Shaders\\DepthPassCube.vertex.cso", ShaderType_VS, device, deviceContext))
			return false;*/
	}

	InputLayoutManager::MicrosoftFailedWithDirectX(device, this->depthSingleShader.GetByteCode(), this->depthSingleShader.GetByteCodeSize());
	this->depthSingleShader.RemoveByteCode();

	/*InputLayoutManager::MicrosoftFailedWithDirectX(device, this->depthCubeShader.GetByteCode(), this->depthCubeShader.GetByteCodeSize());
	this->depthCubeShader.RemoveByteCode();*/
	 
	//Viewport that matches the shadow map dimensions
	this->viewPort.TopLeftX = 0;
	this->viewPort.TopLeftY = 0;
	this->viewPort.MinDepth = 0;
	this->viewPort.MaxDepth = 1;
	this->viewPort.Width = static_cast<float>(width);
	this->viewPort.Height = static_cast<float>(height);

	//Initiation of the SRV depthMap#########
	//Texture description, typeless format used since DSV and SRV interprets the bits differently
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = (UINT)width;
	texDesc.Height = (UINT)height;
	texDesc.CPUAccessFlags = 0;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	//Texture to be used to initiate the DSV and SRV
	ID3D11Texture2D	*depthTex = 0;
	if(FAILED(device->CreateTexture2D(&texDesc, 0, &depthTex)))
		return false;

	//Description and initiation of the DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	
	if (FAILED(device->CreateDepthStencilView(depthTex, &dsvDesc, &this->singleDSV)))
		return false;

	//Description and initiation of the depthMap SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC	srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	if (FAILED(device->CreateShaderResourceView(depthTex, &srvDesc, &this->depthMap)))
		return false;
		

	//Initiation of the SRV depthCube and cubeDSV#########
	//Will reuse the variables above since we just need to change a few settings
	//and recreate the texture2D resource
	depthTex->Release();
	depthTex = NULL;
	//ID3D11Texture2D **cubeDepthTex[6];
	////TEXTURE2D_DESC changed too handle texture cubes
	//texDesc.ArraySize = 6;
	//texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	//for (int i = 0; i < 6; i++)
	//{
	//	cubeDepthTex[i] = NULL;
	//	if (FAILED(device->CreateTexture2D(&texDesc, 0, cubeDepthTex[i])))
	//		return false;
	//}

	////Don't need to change the dsvDesc since it will be cleared between each cube side rendering
	//for (int i = 0; i < 6; i++)
	//{
	//	if (FAILED(device->CreateDepthStencilView(*cubeDepthTex[i], &dsvDesc, &this->cubeDSV[i])))
	//		return false;
	//}
	//

	////SRV_DESC changed too handle texture cubes
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	//srvDesc.TextureCube.MipLevels = texDesc.MipLevels;
	//srvDesc.TextureCube.MostDetailedMip = 0;

	//if (FAILED(device->CreateShaderResourceView(depthTex, &srvDesc, &this->depthCube)))
	//	return false;

	//
	//depthTex->Release();
	//depthTex = NULL;

	if (!ShaderStates::DepthStencilState::GetEnabledDepth(this->device)) return false;
	if (!ShaderStates::RasterizerState::GetNoCullNoMs(this->device)) return false;
	if (!ShaderStates::SamplerState::GetLinear(this->device)) return false;

	this->lightCam = new DepthCamera();
	this->lightCam->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	this->lightCam->SetProjectionMatrix(((DirectX::XM_PI / 180.0f) * 45.0f), ((float)width / (float)height), 0.1f, 10000.0f);
	this->lightCam->UpdateViewMatrix();

	return true;
}

ID3D11ShaderResourceView* DepthPass::GetDepthMapSRVSingle()
{
	return this->depthMap;
}

//ID3D11ShaderResourceView* DepthPass::GetDepthMapSRVCube()
//{
//	return this->depthCube;
//}

//This functions is used for setting the camera for spot and directional lights
void DepthPass::SetCamera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 lookAt)
{
	//Normalize lookAt and calculate the up and right vectors for the camera when rendering from the light's position
	DirectX::XMVECTOR normalizedLook, normalizedUp, normalizedRight;
	DirectX::XMFLOAT3 up, right;
	up.x = lookAt.x - pos.x;
	up.y = lookAt.x - pos.y;
	up.z = lookAt.x - pos.z;

	normalizedLook = DirectX::XMLoadFloat3(&up);

	normalizedLook = DirectX::XMVector3Normalize(normalizedLook);
	DirectX::XMStoreFloat3(&lookAt, normalizedLook);

	if (lookAt.x != 0.0f || lookAt.y != 0.0f)
		up = DirectX::XMFLOAT3(-lookAt.y, lookAt.x, 0.0f);
	else if (lookAt.z < 0)
		up = DirectX::XMFLOAT3(0.0f, -lookAt.z, lookAt.y);
	else
		up = DirectX::XMFLOAT3(0.0f, lookAt.z, lookAt.y);
	


	normalizedUp = DirectX::XMLoadFloat3(&up);

	normalizedRight = DirectX::XMVector3Cross(normalizedUp, normalizedLook);

	DirectX::XMStoreFloat3(&right, normalizedRight);

	//ClearSingleDSV();
	this->lightCam->SetPosition(pos);
	this->lightCam->SetLook(lookAt);
	this->lightCam->SetUp(up);
	this->lightCam->SetRight(right);
	this->lightCam->UpdateViewMatrix();
}

//Camera funtions for rendering depth with a point light
/*
void DepthPass::SetCameraForPositiveX(DirectX::XMFLOAT3 pos)
{
	ClearCubeDSV(PositiveX);
	SetDSVForCube(PositiveX);
	this->lightCam->SetPosition(pos);
	this->lightCam->SetLook(posX);
	this->lightCam->SetUp(posY);
	this->lightCam->SetRight(negZ);
	this->lightCam->UpdateViewMatrix();
}

void DepthPass::SetCameraForNegativeX(DirectX::XMFLOAT3 pos)
{
	ClearCubeDSV(NegativeX);
	SetDSVForCube(NegativeX);
	this->lightCam->SetPosition(pos);
	this->lightCam->SetLook(negX);
	this->lightCam->SetUp(posY);
	this->lightCam->SetRight(posZ);
	this->lightCam->UpdateViewMatrix();
}

void DepthPass::SetCameraForPositiveY(DirectX::XMFLOAT3 pos)
{
	ClearCubeDSV(PositiveY);
	SetDSVForCube(PositiveY);
	this->lightCam->SetPosition(pos);
	this->lightCam->SetLook(posY);
	this->lightCam->SetUp(negZ);	//Can be changed to posZ if not correct
	this->lightCam->SetRight(posX);
	this->lightCam->UpdateViewMatrix();
}

void DepthPass::SetCameraForNegativeY(DirectX::XMFLOAT3 pos)
{
	ClearCubeDSV(NegativeY);
	SetDSVForCube(NegativeY);
	this->lightCam->SetPosition(pos);
	this->lightCam->SetLook(negY);
	this->lightCam->SetUp(negZ);		//Can be changed to posZ if not correct
	this->lightCam->SetRight(negX);
	this->lightCam->UpdateViewMatrix();
}

void DepthPass::SetCameraForPositiveZ(DirectX::XMFLOAT3 pos)
{
	ClearCubeDSV(PositiveZ);
	SetDSVForCube(PositiveZ);
	this->lightCam->SetPosition(pos);
	this->lightCam->SetLook(posZ);
	this->lightCam->SetUp(posY);
	this->lightCam->SetRight(posX);
	this->lightCam->UpdateViewMatrix();
}

void DepthPass::SetCameraForNegativeZ(DirectX::XMFLOAT3 pos)
{
	ClearCubeDSV(NegativeZ);
	SetDSVForCube(NegativeZ);
	this->lightCam->SetPosition(pos);
	this->lightCam->SetLook(negZ);
	this->lightCam->SetUp(posY);
	this->lightCam->SetRight(negX);
	this->lightCam->UpdateViewMatrix();
}
//------------------------------------------------------
*/

DirectX::XMFLOAT4X4 DepthPass::GetCameraView()
{
	return this->lightCam->GetView();
}

DirectX::XMFLOAT4X4 DepthPass::GetCameraProj()
{
	return this->lightCam->GetProj();
}

void DepthPass::ClearSingleDSV()
{
	//this->dc->ClearDepthStencilView(this->singleDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

//void DepthPass::ClearCubeDSV(CubeFace side)
//{
//	this->dc->ClearDepthStencilView(this->cubeDSV[(int)side], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//}

//Private functions

void DepthPass::ApplySingle()
{
	this->dc->ClearDepthStencilView(this->singleDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	this->dc->RSSetViewports(1, &this->viewPort);
	ID3D11RenderTargetView *rtv = { 0 };
	this->dc->OMSetRenderTargets(1, &rtv, this->singleDSV);
	this->depthSingleShader.Apply();
	this->dc->IASetInputLayout(InputLayoutManager::GetLayout_V_VN_VT());
	
	
}

//void DepthPass::ApplyCube()
//{
//	for (int i = 0; i < 6; i++)
//	{
//		this->dc->ClearDepthStencilView(this->cubeDSV[i], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//	}
//	this->dc->RSSetViewports(1, &this->viewPort);
//	this->depthCubeShader.Apply();
//	this->dc->IASetInputLayout(InputLayoutManager::GetLayout_V_VN_VT());
//}
//
//void DepthPass::SetDSVForCube(CubeFace side)
//{
//	ID3D11RenderTargetView *rtv = { 0 };
//	this->dc->OMSetRenderTargets(1, &rtv, this->cubeDSV[(int)side]);
//}


