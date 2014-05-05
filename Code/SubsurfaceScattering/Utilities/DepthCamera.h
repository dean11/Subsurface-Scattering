#ifndef DEPTHCAMERA_H
#define DEPTHCAMERA_H

#include <DirectXMath.h>

class GID;

class DepthCamera
{
	private:
		//DepthCamera coordinates
		//View space origin, x-axis, y-axis, z-axis
		DirectX::XMFLOAT3 position, right, up, lookAt;	

		//View and projection matrices
		DirectX::XMFLOAT4X4 viewMatrix, projMatrix;

	public:
		DepthCamera();
		DepthCamera(const DepthCamera &cam);
		~DepthCamera();

		//Get and set functions for the DepthCamera
		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetRight() const;
		DirectX::XMFLOAT3 GetUp() const;
		DirectX::XMFLOAT3 GetLook() const;

		void SetPosition(DirectX::XMFLOAT3 position);
		void SetRight(DirectX::XMFLOAT3 right);
		void SetUp(DirectX::XMFLOAT3 up);
		void SetLook(DirectX::XMFLOAT3 look);
		void SetProjectionMatrix(float fieldOfView, float aspectRatio, float near, float far);
		void SetProjectionMatrix(DirectX::XMFLOAT4X4 projection);

		//Get the view and proj matrices
		DirectX::XMFLOAT4X4 GetView() const;
		DirectX::XMFLOAT4X4 GetProj() const;
		DirectX::XMFLOAT4X4 GetViewProj();

		//Strafe and walk the DepthCamera
		void Strafe(float strafe);
		void Walk(float walk);
		void SetHeight(float height);

		//Rotate the DepthCamera
		void Roll(float angle);		//Rotation on the x-axis
		void Yaw(float angle);		//Rotation on the y-axis
		void Pitch(float angle);	//Rotation on the z-axis

		//After modifying the DepthCamera position/orientation call to rebuild the view matrix
		void UpdateViewMatrix();
		
};

#endif