#ifndef BATCHELOR_MESHMANAGER_H
#define BATCHELOR_MESHMANAGER_H

#include <d3d11.h>

class Model
{
public:
	struct Mesh
	{
		ID3D11Buffer* vertexBuffer;
	};
	
public:
	Model();
	virtual~Model();

	void Release();
	bool CreateModel(const char path[]);

	Mesh& GetMesh();

private:
	Mesh mesh;
};

#endif // !BATCHELOR_MESHMANAGER_H
