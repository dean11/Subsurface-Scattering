#ifndef DEPTHPASS_H
#define DEPTHPASS_H
#include <d3d11.h>
#include <vector>

#include "..\Shader.h"
#include "..\RenderSurface.h"
#include "ShaderPass.h"
#include "LightPass.h"
#include "..\..\Utilities\DepthCamera.h"
#include "..\..\Scene\Model.h"

namespace Pipeline
{
	class DepthPass : public ShaderPass
	{
	public:
		enum CubeFace
		{
			PositiveX = 0,
			NegativeX = 1,
			PositiveY = 2,
			NegativeY = 3,
			PositiveZ = 4,
			NegativeZ = 5,
		};

		enum DepthMapType
		{
			SingleDepthMap = 0,
			CubeDepthMap = 1,
		};

	public:
		DepthPass();
		virtual~DepthPass();

		void Release();
		void Clear();

		void Apply(DepthMapType depthMapType);
		void Render(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 lookAt);
		//void Render(DirectX::XMFLOAT3 pos, CubeFace face);
		

		bool Initiate(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height, bool forceShaderCompile);
		ID3D11ShaderResourceView* GetDepthMapSRVSingle();
		//ID3D11ShaderResourceView* GetDepthMapSRVCube();

		void ClearSingleDSV();
		//void ClearCubeDSV(CubeFace side);

		void SetCamera(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 lookAt);

		//Camera funtions for rendering depth with a point light
		void SetCameraForPositiveX(DirectX::XMFLOAT3 pos);
		void SetCameraForNegativeX(DirectX::XMFLOAT3 pos);
		void SetCameraForPositiveY(DirectX::XMFLOAT3 pos);
		void SetCameraForNegativeY(DirectX::XMFLOAT3 pos);
		void SetCameraForPositiveZ(DirectX::XMFLOAT3 pos);
		void SetCameraForNegativeZ(DirectX::XMFLOAT3 pos);
		//------------------------------------------------------

		DirectX::XMFLOAT4X4 GetCameraView();
		DirectX::XMFLOAT4X4 GetCameraProj();

	private:
		
		DirectX::XMFLOAT3 posX; 
		DirectX::XMFLOAT3 negX; 
		DirectX::XMFLOAT3 posY; 
		DirectX::XMFLOAT3 negY; 
		DirectX::XMFLOAT3 posZ; 
		DirectX::XMFLOAT3 negZ;

		DepthCamera* lightCam;

		ID3D11ShaderResourceView* depthMap;
		//ID3D11ShaderResourceView* depthCube;

		ID3D11DepthStencilView* singleDSV;
	//	ID3D11DepthStencilView* cubeDSV[6];

		D3D11_VIEWPORT viewPort;

		ID3D11Device* device;
		ID3D11DeviceContext* dc;

		Shader depthSingleShader;
		//Shader depthCubeShader;

	private:
		void ApplySingle();
		/*void ApplyCube();
		void SetDSVForCube(CubeFace side);*/

	};
}
#endif