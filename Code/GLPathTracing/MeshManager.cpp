#include "MeshManager.h"

using namespace Mesh;

static int meshIdCounter = 0;

void Model::SetPosition(float pos[3])
{

}
Model::Model()
{

}
Model::~Model()
{

}


ModelManager::ModelManager()
{

}
ModelManager::~ModelManager()
{

}
Model ModelManager::CreateModel(const char path[])
{
	ModelManager::Mesh m;
	m.id = meshIdCounter++;
	
	return Model();
}
void ModelManager::RenderModel(Model& model)
{

}

