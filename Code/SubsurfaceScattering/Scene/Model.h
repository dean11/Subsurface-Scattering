#ifndef BATCHELOR_MESHMANAGER_H
#define BATCHELOR_MESHMANAGER_H

#include <d3d11_2.h>

class Model
{
public:
	struct Mesh
	{
		unsigned int vertexCount;
		unsigned int vertexStride;
		ID3D11Buffer* vertexBuffer;
	};
	
public:
	Model();
	virtual~Model();

	void Release();
	bool CreateModel(const char path[], ID3D11Device* device);

	Mesh& GetMesh();

private:
	Mesh mesh;
};

#endif // !BATCHELOR_MESHMANAGER_H
