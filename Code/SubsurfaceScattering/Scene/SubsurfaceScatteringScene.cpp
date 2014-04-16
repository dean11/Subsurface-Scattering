#include "SubsurfaceScatteringScene.h"

SubsurfaceScatteringScene::SubsurfaceScatteringScene()
{

}
SubsurfaceScatteringScene::~SubsurfaceScatteringScene()
{

}

void SubsurfaceScatteringScene::Frame(float delta)
{
	Pipeline::PipelineManager::Instance().ApplyGeometryPass(true);

	Pipeline::PipelineManager::Instance().ApplySSSPass(true);

	Pipeline::PipelineManager::Instance().ApplySSSPass(true);

	Pipeline::PipelineManager::Instance().Present();
}
void SubsurfaceScatteringScene::Initiate(ID3D11Device* device, ID3D11DeviceContext *dc)
{
	this->device = device;
	this->deviceContext = dc;

}

