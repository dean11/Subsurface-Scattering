#ifndef BATCHELOR_MESHMANAGER_H
#define BATCHELOR_MESHMANAGER_H

#include <d3d11_2.h>
#include <D3DTK\DDSTextureLoader.h>
#include "..\Utilities\Util.h"
#include <D3DTK\SimpleMath.h>

using namespace DirectX;
class Model
{
public:
	struct Mesh
	{
		unsigned int vertexCount;
		unsigned int vertexStride;
		unsigned int materialLayerCount;
		ID3D11Buffer* vertexBuffer;
		ID3D11ShaderResourceView* diffuse;
		ID3D11ShaderResourceView* thickness;
		const SimpleMath::Vector4* materialLayers;
	};
	
public:
	Model();
	virtual~Model();

	virtual void Release();
	virtual bool CreateModel(const char path[], ID3D11Device* device, const SimpleMath::Vector4 materialLayer[], int layerCount);

	void DrawModel(ID3D11DeviceContext* dc, bool useTextures = true);

	Mesh& GetMesh();
	DirectX::XMMATRIX GetWorld();
	DirectX::XMMATRIX GetWorld() const;

	DirectX::XMMATRIX GetWorldInversTranspose();
	DirectX::XMMATRIX GetWorldInversTranspose() const;

	void SetWorld(DirectX::XMFLOAT4X4 world);
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3& v);
	void Forward(float val);
	void Up(float val);
	void Right(float val);
	//void SetWorldInversTranspose(DirectX::XMFLOAT4X4 worldInvTrans); SÄTTA DENNA I SAMBAND MED NY WORLD?
	DirectX::SimpleMath::Vector3 GetPosition();
	void Rotate(const SimpleMath::Vector3& angle);
	void SetScale(float x, float y, float z);
	void SetScale(float s);
	void ToggleVisibility(){this->isVisible = !this->isVisible;}
	void SetVisibility(bool vis) {this->isVisible = vis;}
	bool GetVisibility(){return this->isVisible;}

protected:
	Mesh mesh;
	DirectX::SimpleMath::Matrix world;
	bool isVisible;
};

#endif // !BATCHELOR_MESHMANAGER_H
