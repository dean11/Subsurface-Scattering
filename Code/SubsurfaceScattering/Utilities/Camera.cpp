#include "Camera.h"
#include <D3DTK\SimpleMath.h>
using namespace DirectX;

Camera::Camera()
{
	this->positionX = 0.0f;
	this->positionY = 0.0f;
	this->positionZ = 0.0f;

	this->rotationX = 0.0f;
	this->rotationY = 0.0f;
	this->rotationZ = 0.0f;

	this->projNear = 0.0f;
	this->projFar = 0.0f;

	this->lookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
}
//Camera::Camera(const Camera& other)
//{
//	this->positionX = other.positionX;
//	this->positionY = other.positionY;
//	this->positionZ = other.positionZ;
//
//	this->rotationX = other.rotationX;
//	this->rotationY = other.rotationY;
//	this->rotationZ = other.rotationZ;
//
//	this->projNear = other.projNear;
//	this->projFar = other.projFar;
//
//	this->lookAt = other.lookAt;
//
//	this->viewMatrix = other.viewMatrix;
//	this->projectionMatrix = other.projectionMatrix;
//	this->orthogonalMatrix = other.orthogonalMatrix;
//}
Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	this->positionX = x;
	this->positionY = y;
	this->positionZ = z;
}
void Camera::SetPosition(DirectX::XMFLOAT3 position)
{
	this->positionX = position.x;
	this->positionY = position.y;
	this->positionZ = position.z;
}
void Camera::SetPositionX(float x)
{
	this->positionX = x;
}
void Camera::SetPositionY(float y)
{
	this->positionY = y;
}
void Camera::SetPositionZ(float z)
{
	this->positionZ = z;
}
void Camera::RelativeForward(float speed)
{
	DirectX::XMFLOAT3 norm;
	XMStoreFloat3(&norm, DirectX::XMVector3Normalize(XMLoadFloat3(&this->GetForward())));

	this->positionX += norm.x * speed;
	this->positionY += norm.y * speed;
	this->positionZ += norm.z * speed;

	//D3DXMatrixOrthoLH(&this->orthogonalMatrix, width, height, nearPlane, farPlane);
}
void Camera::RelativeRight(float speed)
{
	DirectX::XMFLOAT3 norm;
	XMStoreFloat3(&norm, DirectX::XMVector3Normalize(XMLoadFloat3(&this->GetRight())));

	this->positionX += norm.x*speed;
	this->positionY += norm.y*speed;
	this->positionZ += norm.z*speed;
}
void Camera::RelativeUp(float speed)
{
	DirectX::XMFLOAT3 norm;
	XMStoreFloat3(&norm, DirectX::XMVector3Normalize(XMLoadFloat3(&this->GetUp())));

	this->positionX += norm.x*speed;
	this->positionY += norm.y*speed;
	this->positionZ += norm.z*speed;
}
void Camera::RelativeParallelForward(float speed)
{
	DirectX::XMFLOAT3 norm;
	XMStoreFloat3(&norm, DirectX::XMVector3Normalize(XMLoadFloat3(&this->GetParallelForward())));

	this->positionX += norm.x*speed;
	this->positionY += norm.y*speed;
	this->positionZ += norm.z*speed;
}
void Camera::RelativeParallelRight(float speed)
{
	DirectX::XMFLOAT3 norm;
	XMStoreFloat3(&norm, DirectX::XMVector3Normalize(XMLoadFloat3(&this->GetParallelRight())));

	this->positionX += norm.x*speed;
	this->positionY += norm.y*speed;
	this->positionZ += norm.z*speed;
}

void Camera::SetRotation(float x, float y, float z)
{
	this->rotationX = x;
	this->rotationY = y;
	this->rotationZ = z;
}
void Camera::SetRotation(DirectX::XMFLOAT3 v)
{
	this->rotationX = v.x;
	this->rotationY = v.y;
	this->rotationZ = v.z;
}
void Camera::SetRotationX(float x)
{
	this->rotationX = x;
}
void Camera::SetRotationY(float y)
{
	this->rotationY = y;
}
void Camera::SetRotationZ(float z)
{
	this->rotationZ = z;
}
void Camera::SetLookAt(DirectX::XMFLOAT3 lookAt)
{
	this->lookAt = lookAt;
}
void Camera::SetUp(DirectX::XMFLOAT3 up)
{
	this->viewMatrix._12 = up.x;
	this->viewMatrix._22 = up.y;
	this->viewMatrix._32 = up.z;
}
void Camera::SetRight(DirectX::XMFLOAT3 right)
{
	this->viewMatrix._11 = right.x;
	this->viewMatrix._21 = right.y;
	this->viewMatrix._31 = right.z;
}
void Camera::RelativePitch(float degrees)
{
	this->rotationX += degrees;
}
void Camera::RelativeYaw(float degrees)
{
	this->rotationY += degrees;
}
void Camera::RelativeRoll(float degrees)
{
	this->rotationZ += degrees;
}

void Camera::SetViewMatrix(DirectX::XMFLOAT4X4 view)
{
	this->positionX = view._41;
	this->positionY = view._42;
	this->positionZ = view._43;

	this->lookAt.x = view._31;
	this->lookAt.y = view._32;
	this->lookAt.z = view._33;

	this->viewMatrix = view;
}
void Camera::SetViewMatrix(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT3 right)
{
	this->positionX = position.x;
	this->positionY = position.y;
	this->positionZ = position.z;

	this->lookAt = direction;

	XMVECTOR p = XMLoadFloat3(&position);
	XMVECTOR d = XMLoadFloat3(&direction);
	XMVECTOR u = XMVector3Cross(d, XMLoadFloat3(&right));

	XMStoreFloat4x4 (&this->viewMatrix, DirectX::XMMatrixLookToLH(p, d, u));
}
void Camera::SetProjectionMatrix(DirectX::XMFLOAT4X4 projection)
{
	this->projectionMatrix = projection;
}
void Camera::SetProjectionMatrix(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	this->projFar = farPlane;
	this->projNear = nearPlane;
	//D3DXMatrixPerspectiveFovLH(&this->projectionMatrix, fieldOfView, aspectRatio, nearPlane, farPlane);
	XMStoreFloat4x4(&this->projectionMatrix, XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane));
}
void Camera::SetOrthogonalMatrix(float width, float height, float nearPlane, float farPlane)
{
	XMStoreFloat4x4(&this->orthogonalMatrix, XMMatrixOrthographicLH(width, height, nearPlane, farPlane));
	
	//D3DXMatrixOrthoLH(&this->orthogonalMatrix, width, height, nearPlane, farPlane);
}
void Camera::SetOrthogonalMatrix(DirectX::XMFLOAT4X4 projection)
{
	this->orthogonalMatrix = projection;
}

void Camera::Render()
{
	DirectX::XMFLOAT3 up, position, xAxis, yAxis, zAxis;
	float yaw, pitch, roll;
	
	//Set position
	position.x = this->positionX;
	position.y = this->positionY;
	position.z = this->positionZ;

	//Point "up"
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//Point to 1 z(default)
	this->lookAt.x = 0.0f;
	this->lookAt.y = 0.0f;
	this->lookAt.z = 1.0f;

	//Set rotations
	pitch = this->rotationX*0.0174532925f;
	yaw = this->rotationY*0.0174532925f;
	roll = this->rotationZ*0.0174532925f;


	//Create rotation matrix
	FXMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//Transform lookAt and up so they are rotated accordingly
	XMStoreFloat3(&this->lookAt, XMVector3TransformCoord(XMLoadFloat3(&this->lookAt), rotationMatrix));
	XMStoreFloat3(&up, XMVector3TransformCoord(XMLoadFloat3(&up), rotationMatrix));
	
	

	//Create the view matrix from the three updated vectors
	XMStoreFloat3(&zAxis, XMVector3Normalize(XMLoadFloat3(&this->lookAt)));
	//D3DXVec3Normalize(&zAxis, &this->lookAt);
	XMStoreFloat3(&xAxis, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&zAxis))));
	//D3DXVec3Normalize(&xAxis, D3DXVec3Cross(&xAxis, &up, &zAxis));
	XMStoreFloat3(&yAxis, XMVector3Cross(XMLoadFloat3(&zAxis), XMLoadFloat3(&xAxis)));
	//D3DXVec3Cross(&yAxis, &zAxis, &xAxis);


	//Translate the rotated camera position to the location of the viewer.
	this->lookAt.x += position.x;
	this->lookAt.y += position.y;
	this->lookAt.z += position.z;

	
	float tmp = 0;
	this->viewMatrix._11 = xAxis.x;
	this->viewMatrix._21 = xAxis.y;
	this->viewMatrix._31 = xAxis.z;
	//this->viewMatrix._41 = -D3DXVec3Dot(&xAxis, &position);
	XMStoreFloat(&tmp, XMVector3Dot(XMLoadFloat3(&xAxis), XMLoadFloat3(&position)));
	this->viewMatrix._41 = -tmp;

	this->viewMatrix._12 = yAxis.x;
	this->viewMatrix._22 = yAxis.y;
	this->viewMatrix._32 = yAxis.z;
	//this->viewMatrix._42 = -D3DXVec3Dot(&yAxis, &position);
	XMStoreFloat(&tmp, XMVector3Dot(XMLoadFloat3(&yAxis), XMLoadFloat3(&position)));
	this->viewMatrix._42 = -tmp;

	this->viewMatrix._13 = zAxis.x;
	this->viewMatrix._23 = zAxis.y;
	this->viewMatrix._33 = zAxis.z;
	//this->viewMatrix._43 = -D3DXVec3Dot(&zAxis, &position);
	XMStoreFloat(&tmp, XMVector3Dot(XMLoadFloat3(&zAxis), XMLoadFloat3(&position)));
	this->viewMatrix._43 = -tmp;

	this->viewMatrix._14 = 0.0f;
	this->viewMatrix._24 = 0.0f;
	this->viewMatrix._34 = 0.0f;
	this->viewMatrix._44 = 1.0f;


}

DirectX::XMFLOAT3 Camera::GetPosition() const
{
	return DirectX::XMFLOAT3(this->positionX, this->positionY, this->positionZ);
}
DirectX::XMFLOAT3 Camera::GetRotation() const
{
	return DirectX::XMFLOAT3(this->rotationX, this->rotationY, this->rotationZ);
}

DirectX::XMFLOAT3 Camera::GetLookAt() const
{
	return DirectX::XMFLOAT3(this->lookAt.x, this->lookAt.y, this->lookAt.z);
}
DirectX::XMFLOAT4X4 Camera::GetViewMatrix() const
{
	return this->viewMatrix;
}
DirectX::XMFLOAT4X4 Camera::GetInverseViewMatrix() const
{
	DirectX::XMFLOAT4X4 inv;
	XMStoreFloat4x4(&inv, XMMatrixInverse(0, XMLoadFloat4x4(&this->viewMatrix)));
	//D3DXMatrixInverse(&inv, 0, &this->viewMatrix);
	return inv;
}
DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}
DirectX::XMFLOAT4X4 Camera::GetInverseProjectionMatrix() const
{
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixInverse(0, DirectX::XMLoadFloat4x4(&this->projectionMatrix)));
	return m;
}
DirectX::XMFLOAT4X4	Camera::GetOrthogonalMatrix() const
{
	return this->orthogonalMatrix;
}
DirectX::XMFLOAT4X4 Camera::GetViewProjection() const
{
	DirectX::XMFLOAT4X4 vp;

	//D3DXMatrixMultiply(&vp, &this->viewMatrix, &this->projectionMatrix);
	XMStoreFloat4x4(&vp, XMMatrixMultiply(XMLoadFloat4x4(&this->viewMatrix), XMLoadFloat4x4(&this->projectionMatrix)));

	return vp;
}

DirectX::XMFLOAT3	Camera::GetRight() const
{
	return DirectX::XMFLOAT3(this->viewMatrix._11, this->viewMatrix._21, this->viewMatrix._31);
}
DirectX::XMFLOAT3 Camera::GetForward() const
{
	return DirectX::XMFLOAT3(this->viewMatrix._13, this->viewMatrix._23, this->viewMatrix._33);
}
DirectX::XMFLOAT3 Camera::GetUp() const
{
	return DirectX::XMFLOAT3(this->viewMatrix._12, this->viewMatrix._22, this->viewMatrix._32);
}
DirectX::XMFLOAT3	Camera::GetParallelForward() const
{
	DirectX::XMFLOAT3 up, returnedValue;

	//Point "up"
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//D3DXVec3Cross(&returnedValue, &GetRight(), &up);
	XMStoreFloat3(&returnedValue, XMVector3Cross(XMLoadFloat3(&GetRight()), XMLoadFloat3(&up)));

	return returnedValue;
}
DirectX::XMFLOAT3 Camera::GetParallelRight() const
{
	DirectX::XMFLOAT3 up, returnedValue;

	//Point "up"
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//D3DXVec3Cross(&returnedValue, &up, &GetForward());
	XMStoreFloat3(&returnedValue, XMVector3Cross(XMLoadFloat3(&up), XMLoadFloat3(&GetForward())));

	return returnedValue;
}


float Camera::GetFarZ() const
{
	return this->projFar;
}
float Camera::GetNearZ() const
{
	return this->projNear;
}

/*
void Camera::ConstructViewFrustum(ViewFrustum& frustum)
{
	float zMinimum, r;
	DirectX::XMFLOAT4X4 matrix;
		
	// Calculate the minimum Z distance in the frustum.
	zMinimum = -this->projectionMatrix._43 / this->projectionMatrix._33;
	r = this->projFar / (this->projFar - zMinimum);
	this->projectionMatrix._33 = r;
	this->projectionMatrix._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	D3DXMatrixMultiply(&matrix, &this->viewMatrix, &this->projectionMatrix);

	// Calculate near plane of frustum.
	frustum.planes[0].a = matrix._14 + matrix._13;
	frustum.planes[0].b = matrix._24 + matrix._23;
	frustum.planes[0].c = matrix._34 + matrix._33;
	frustum.planes[0].d = matrix._44 + matrix._43;
	D3DXPlaneNormalize(&frustum.planes[0], &frustum.planes[0]);

	// Calculate far plane of frustum.
	frustum.planes[1].a = matrix._14 - matrix._13; 
	frustum.planes[1].b = matrix._24 - matrix._23;
	frustum.planes[1].c = matrix._34 - matrix._33;
	frustum.planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&frustum.planes[1], &frustum.planes[1]);

	// Calculate left plane of frustum.
	frustum.planes[2].a = matrix._14 + matrix._11; 
	frustum.planes[2].b = matrix._24 + matrix._21;
	frustum.planes[2].c = matrix._34 + matrix._31;
	frustum.planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&frustum.planes[2], &frustum.planes[2]);

	// Calculate right plane of frustum.
	frustum.planes[3].a = matrix._14 - matrix._11; 
	frustum.planes[3].b = matrix._24 - matrix._21;
	frustum.planes[3].c = matrix._34 - matrix._31;
	frustum.planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&frustum.planes[3], &frustum.planes[3]);

	// Calculate top plane of frustum.
	frustum.planes[4].a = matrix._14 - matrix._12; 
	frustum.planes[4].b = matrix._24 - matrix._22;
	frustum.planes[4].c = matrix._34 - matrix._32;
	frustum.planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&frustum.planes[4], &frustum.planes[4]);

	// Calculate bottom plane of frustum.
	frustum.planes[5].a = matrix._14 + matrix._12;
	frustum.planes[5].b = matrix._24 + matrix._22;
	frustum.planes[5].c = matrix._34 + matrix._32;
	frustum.planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&frustum.planes[5], &frustum.planes[5]);

	//Calculate sphere around frustum for faster culling
	float length = this->projFar - this->projNear;
	float height = length*tan((float)D3DX_PI*0.5f*0.5f);
	float width = height*(800/600);

	DirectX::XMFLOAT3 p(0.0f,0.0f, 1+length*0.5f);
	DirectX::XMFLOAT3 q(width, height, length);

	DirectX::XMFLOAT3 vDiff(p-q);

	frustum.sphere.radius = D3DXVec3Length(&vDiff);
	frustum.sphere.center = DirectX::XMFLOAT3(this->positionX, this->positionY, this->positionZ) + (DirectX::XMFLOAT3(this->GetLookAt().x - this->positionX, this->GetLookAt().y - this->positionY, this->GetLookAt().z - this->positionZ)*(length*0.5f));  
}
*/

/*
void Camera::ConstructViewFrustum(ViewFrustum& frustum) 
{
	DirectX::XMFLOAT4X4 matrix;
	float zMinimum, r;
	// Calculate the minimum Z distance in the frustum.
	zMinimum = -this->projectionMatrix._43 / this->projectionMatrix._33;
	r = this->projFar / (this->projFar - zMinimum);
	this->projectionMatrix._33 = r;
	this->projectionMatrix._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	D3DXMatrixMultiply(&matrix, &this->viewMatrix, &this->projectionMatrix);

	// Calculate near plane of frustum.
	frustum.planes[0].a = matrix._13;
	frustum.planes[0].b = matrix._23;
	frustum.planes[0].c = matrix._33;
	frustum.planes[0].d = matrix._43;
	D3DXPlaneNormalize(&frustum.planes[0], &frustum.planes[0]);

	// Calculate far plane of frustum.
	frustum.planes[1].a = matrix._14 - matrix._13; 
	frustum.planes[1].b = matrix._24 - matrix._23;
	frustum.planes[1].c = matrix._34 - matrix._33;
	frustum.planes[1].d = matrix._44 - matrix._43;
	D3DXPlaneNormalize(&frustum.planes[1], &frustum.planes[1]);

	// Calculate left plane of frustum.
	frustum.planes[2].a = matrix._14 + matrix._11; 
	frustum.planes[2].b = matrix._24 + matrix._21;
	frustum.planes[2].c = matrix._34 + matrix._31;
	frustum.planes[2].d = matrix._44 + matrix._41;
	D3DXPlaneNormalize(&frustum.planes[2], &frustum.planes[2]);

	// Calculate right plane of frustum.
	frustum.planes[3].a = matrix._14 - matrix._11; 
	frustum.planes[3].b = matrix._24 - matrix._21;
	frustum.planes[3].c = matrix._34 - matrix._31;
	frustum.planes[3].d = matrix._44 - matrix._41;
	D3DXPlaneNormalize(&frustum.planes[3], &frustum.planes[3]);

	// Calculate top plane of frustum.
	frustum.planes[4].a = matrix._14 - matrix._12; 
	frustum.planes[4].b = matrix._24 - matrix._22;
	frustum.planes[4].c = matrix._34 - matrix._32;
	frustum.planes[4].d = matrix._44 - matrix._42;
	D3DXPlaneNormalize(&frustum.planes[4], &frustum.planes[4]);

	// Calculate bottom plane of frustum.
	frustum.planes[5].a = matrix._14 + matrix._12;
	frustum.planes[5].b = matrix._24 + matrix._22;
	frustum.planes[5].c = matrix._34 + matrix._32;
	frustum.planes[5].d = matrix._44 + matrix._42;
	D3DXPlaneNormalize(&frustum.planes[5], &frustum.planes[5]);

	//Calculate sphere around frustum for faster culling
	float length = this->projFar - this->projNear;
	float height = length*tan((float)D3DX_PI*0.5f*0.5f);
	float width = height*(800/600);

	DirectX::XMFLOAT3 p(0.0f,0.0f, 1+length*0.5f);
	DirectX::XMFLOAT3 q(width, height, length);

	DirectX::XMFLOAT3 vDiff(p-q);

	frustum.sphere.radius = D3DXVec3Length(&vDiff);
	frustum.sphere.center = DirectX::XMFLOAT3(this->positionX, this->positionY, this->positionZ) + (DirectX::XMFLOAT3(this->GetLookAt().x - this->positionX, this->GetLookAt().y - this->positionY, this->GetLookAt().z - this->positionZ)*(length*0.5f));  
}
*/