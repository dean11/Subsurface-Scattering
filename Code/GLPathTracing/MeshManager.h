#ifndef BATCHELOR_MESHMANAGER_H
#define BATCHELOR_MESHMANAGER_H

#include <d3d11.h>
#include <vector>

namespace Mesh
{
	
	class Model
	{
	public:
		void SetPosition(float pos[3]);

	private:
		friend class ModelManager;
		int model;

		Model();
		virtual~Model();
	};

	class ModelManager
	{
	private:
		struct Mesh
		{
			Mesh()
			{
				vertexBuffer = 0;
				id = -1;
			}

			ID3D11Buffer* vertexBuffer;
			int id;
		};
	
	public:
		ModelManager();
		virtual~ModelManager();

		Model CreateModel(const char path[]);
		
		void RenderModel(Model& model);

	private:
		std::vector<ModelManager::Mesh> models;

	};
}

#endif // !BATCHELOR_MESHMANAGER_H
