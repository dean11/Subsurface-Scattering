#include "DepthCamera.h"

DepthCamera::DepthCamera()
{
	this->position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	this->up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	this->lookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	
}

DepthCamera::DepthCamera(const DepthCamera &cam)	
{
	this->position = cam.position;
	this->right = cam.right;
	this->up = cam.up;
	this->lookAt = cam.lookAt;
	this->viewMatrix = cam.viewMatrix;
	this->projMatrix = cam.projMatrix;
}

DepthCamera::~DepthCamera()
{
}

//Get and set functions for the DepthCamera
DirectX::XMFLOAT3 DepthCamera::GetPosition() const
{
	return this->position;
}

DirectX::XMFLOAT3 DepthCamera::GetRight() const
{
	return this->right;
}

DirectX::XMFLOAT3 DepthCamera::GetUp() const
{
	return this->up;
}

DirectX::XMFLOAT3 DepthCamera::GetLook() const
{
	return this->lookAt;
}

void DepthCamera::SetPosition(DirectX::XMFLOAT3 position)
{
	this->position = position;
}

void DepthCamera::SetRight(DirectX::XMFLOAT3 right)
{
	this->right = right;
}

void DepthCamera::SetUp(DirectX::XMFLOAT3 up)
{
	this->up = up;
}

void DepthCamera::SetLook(DirectX::XMFLOAT3 look)
{
	this->lookAt = look;
}

void DepthCamera::SetProjectionMatrix(float fieldOfView, float aspectRatio, float near, float far)
{
	XMStoreFloat4x4(&this->projMatrix, DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, near, far));
}

void DepthCamera::SetProjectionMatrix(DirectX::XMFLOAT4X4 projection)
{
	this->projMatrix = projection;
}

//Get the view and proj matrices
DirectX::XMFLOAT4X4 DepthCamera::GetView() const
{
	return this->viewMatrix;
}

DirectX::XMFLOAT4X4 DepthCamera::GetProj() const
{
	return this->projMatrix;
}

DirectX::XMFLOAT4X4 DepthCamera::GetViewProj() 
{
	DirectX::XMFLOAT4X4 vp;
	XMStoreFloat4x4(&vp, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&this->viewMatrix), DirectX::XMLoadFloat4x4(&this->projMatrix)));
	return vp;
}

//Strafe and walk the DepthCamera
void DepthCamera::Strafe(float strafe)
{
	
	this->position.x += strafe*GetRight().x;
	this->position.y += strafe*GetRight().y;
	this->position.z += strafe*GetRight().z;
}

void DepthCamera::Walk(float walk)
{
	this->position.x += walk*GetLook().x;
	this->position.y += walk*GetLook().y;
	this->position.z += walk*GetLook().z;
}

void DepthCamera::SetHeight(float height)
{
	this->position.y = height;
}

//Rotation on the z-axis
void DepthCamera::Roll(float angle)
{

}

//Rotation on the y-axis
void DepthCamera::Yaw(float angle)
{
	DirectX::XMMATRIX rotation;
	rotation = DirectX::XMMatrixRotationRollPitchYaw(0.0f, angle, 0.0f);
	
	DirectX::XMStoreFloat3(&this->lookAt, DirectX::XMVector3TransformCoord(XMLoadFloat3(&this->lookAt), rotation));
	DirectX::XMStoreFloat3(&this->up, DirectX::XMVector3TransformCoord(XMLoadFloat3(&this->up), rotation));
	DirectX::XMStoreFloat3(&this->right, DirectX::XMVector3TransformCoord(XMLoadFloat3(&this->right), rotation));
}

//Rotation on the x-axis
void DepthCamera::Pitch(float angle)	
{
	DirectX::XMMATRIX rotation;
	rotation = DirectX::XMMatrixRotationRollPitchYaw(angle, 0.0f, 0.0f);

	DirectX::XMStoreFloat3(&this->lookAt, DirectX::XMVector3TransformCoord(XMLoadFloat3(&this->lookAt), rotation));
	DirectX::XMStoreFloat3(&this->up, DirectX::XMVector3TransformCoord(XMLoadFloat3(&this->up), rotation));
	DirectX::XMStoreFloat3(&this->right, DirectX::XMVector3TransformCoord(XMLoadFloat3(&this->right), rotation));
}

//After modifying the DepthCamera position/orientation call to rebuild the view matrix
void DepthCamera::UpdateViewMatrix()
{
	//Right, Up, Look
	DirectX::XMFLOAT3 R, U, L;

	//Orthonormalize the Right, Up and Look vectors
	DirectX::XMStoreFloat3(&L, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&this->lookAt))); //Normalize the Look vector
	DirectX::XMStoreFloat3(&U, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&L), DirectX::XMLoadFloat3(&this->right)))); //Normalize the Up vector
	DirectX::XMStoreFloat3(&R, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&U), DirectX::XMLoadFloat3(&L))); //Use cross product of normalized U and L to get the Right vector							

	//Fill in the view matrix entries
	this->right  = R;
	this->up	 = U;
	this->lookAt = L;
	float temp;

	this->viewMatrix._11 = this->right.x;
	this->viewMatrix._21 = this->right.y;
	this->viewMatrix._31 = this->right.z;
	DirectX::XMStoreFloat(&temp, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&R), DirectX::XMLoadFloat3(&this->position)));
	this->viewMatrix._41 = -temp;
			 
	this->viewMatrix._12 = this->up.x;
	this->viewMatrix._22 = this->up.y;
	this->viewMatrix._32 = this->up.z;
	DirectX::XMStoreFloat(&temp, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&U), DirectX::XMLoadFloat3(&this->position)));
	this->viewMatrix._42 = -temp;
			 
	this->viewMatrix._13 = this->lookAt.x;
	this->viewMatrix._23 = this->lookAt.y;
	this->viewMatrix._33 = this->lookAt.z;
	DirectX::XMStoreFloat(&temp, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&L), DirectX::XMLoadFloat3(&this->position)));
	this->viewMatrix._43 = -temp;
			 
	this->viewMatrix._14 = 0.0f;
	this->viewMatrix._24 = 0.0f;
	this->viewMatrix._34 = 0.0f;
	this->viewMatrix._44 = 1.0f;
	
}
