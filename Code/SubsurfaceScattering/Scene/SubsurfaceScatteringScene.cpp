#include "SubsurfaceScatteringScene.h"

SubsurfaceScatteringScene::SubsurfaceScatteringScene(SSSInitDesc& desc)
{
	this->desc = desc;
}
SubsurfaceScatteringScene::~SubsurfaceScatteringScene()
{

}


void SubsurfaceScatteringScene::Frame(float delta)
{
	Pipeline::PipelineManager::Instance().ApplyGeometryPass(true);
	{
		Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(this->mainCam.GetViewMatrix(), this->mainCam.GetProjectionMatrix());

		for (size_t i = 0; i < this->models.size(); i++)
		{
			UINT off = 0;
			this->deviceContext->IASetVertexBuffers(0, 1, &this->models[i].GetMesh().vertexBuffer, &this->models[i].GetMesh().vertexStride, &off);
			Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(this->models[i].GetWorld(), this->models[i].GetWorldInversTranspose());
			this->deviceContext->Draw(this->models[i].GetMesh().vertexCount, 0);
		}
	}
	
	//Pipeline::PipelineManager::Instance().ApplyLightPass(true);

	//Pipeline::PipelineManager::Instance().ApplySSSPass(true);

	Pipeline::PipelineManager::Instance().Present();
}
bool SubsurfaceScatteringScene::Initiate(ID3D11Device* device, ID3D11DeviceContext *dc)
{
	this->device = device;
	this->deviceContext = dc;

	Model bth;
	if (!bth.CreateModel("Models\\bth.obj", device))
		return false;
	this->models.push_back(bth);

	Model sphere;
	if (!sphere.CreateModel("Models\\sphere.obj", device))
		return false;
	this->models.push_back(sphere);

	this->mainCam.SetPosition(0.0f, 0.0f, -10.0f);
	this->mainCam.SetProjectionMatrix(((20.0f * DirectX::XM_PI) / 180.0f), (float)(this->desc.width / this->desc.height), 0.1f, 1000.0f);
	this->mainCam.Render();

	return true;
}

