#include "Model.h"

Model::Model()
{

}
Model::~Model()
{

}
void Model::Release()
{

}
bool Model::CreateModel(const char path[])
{

	return true;
}
Model::Mesh& Model::GetMesh()
{
	return this->mesh;
}

