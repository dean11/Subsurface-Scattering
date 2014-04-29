#ifndef CAMERA_H
#define CAMERA_H

#include <DirectXMath.h>

class GID;

class Camera
{
	private:
		float positionX, positionY, positionZ;
		float rotationX, rotationY, rotationZ;
		float projNear, projFar;

		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projectionMatrix;
		DirectX::XMFLOAT4X4 orthogonalMatrix;
		DirectX::XMFLOAT3 lookAt;

	public:
		Camera();
		Camera(const Camera& other);
		~Camera();


		void SetPosition(float x, float y, float z);
		void SetPosition(DirectX::XMFLOAT3 position);
		void SetPositionX(float x);
		void SetPositionY(float y);
		void SetPositionZ(float z);
		void RelativeForward(float speed);
		void RelativeRight(float speed);
		void RelativeUp(float speed);
		void RelativeParallelForward(float speed);
		void RelativeParallelRight(float speed);

		/**Angles are set in degrees*/
		void SetRotation(float x, float y, float z);
		void SetRotation(DirectX::XMFLOAT3 v);
		void SetRotationX(float x);
		void SetRotationY(float y);
		void SetRotationZ(float z);
		void RelativePitch(float degrees);
		void RelativeYaw(float degrees);
		void RelativeRoll(float degrees);

		void SetViewMatrix(DirectX::XMFLOAT4X4 view);
		void SetProjectionMatrix(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);
		void SetProjectionMatrix(DirectX::XMFLOAT4X4 projection);
		void SetOrthogonalMatrix(float width, float height, float nearPlane, float farPlane);
		void SetOrthogonalMatrix(DirectX::XMFLOAT4X4 projection);

		DirectX::XMFLOAT3 GetPosition() const;
		DirectX::XMFLOAT3 GetRotation() const;
		DirectX::XMFLOAT3 GetLookAt() const;

		void Render();
		void UpdatePosition();

		DirectX::XMFLOAT4X4	GetViewMatrix() const;
		DirectX::XMFLOAT4X4	GetInverseViewMatrix() const;
		DirectX::XMFLOAT4X4	GetProjectionMatrix() const;
		DirectX::XMFLOAT4X4 GetInverseProjectionMatrix() const;
		DirectX::XMFLOAT4X4	GetOrthogonalMatrix() const;
		DirectX::XMFLOAT4X4	GetViewProjection() const;
		DirectX::XMFLOAT3	GetForward() const;
		DirectX::XMFLOAT3	GetRight() const;
		DirectX::XMFLOAT3	GetUp() const;
		/**Use these two functions to get vectors that are parallel to the X and Z-axes (useful for ground movement*/
		DirectX::XMFLOAT3	GetParallelForward() const;
		DirectX::XMFLOAT3	GetParallelRight() const;

		float GetFarZ() const;
		float GetNearZ() const;

		//void ConstructViewFrustum(ViewFrustum& frustum);
		 

};

#endif