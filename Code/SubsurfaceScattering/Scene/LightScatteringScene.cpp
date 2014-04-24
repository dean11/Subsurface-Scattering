#include "LightScatteringScene.h"

LightScatteringScene::LightScatteringScene(LSSInitDesc& desc)
{
	this->desc = desc;
	this->sphereMap = SphereMap();
}

LightScatteringScene::~LightScatteringScene()
{

}

void LightScatteringScene::Frame(float delta)
{
	Pipeline::PipelineManager::Instance().ApplyGeometryPass(true);
	{
		Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(this->mainCam->GetViewMatrix(), this->mainCam->GetProjectionMatrix());

		for (size_t i = 0; i < this->models.size(); i++)
		{
			UINT off = 0;
			this->dc->IASetVertexBuffers(0, 1, &this->models[i].GetMesh().vertexBuffer, &this->models[i].GetMesh().vertexStride, &off);
			this->dc->PSSetShaderResources(0, 1, &this->models[i].GetMesh().diffuse);
			Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(this->models[i].GetWorld(), this->models[i].GetWorldInversTranspose());
			this->dc->Draw(this->models[i].GetMesh().vertexCount, 0);
		}
		ID3D11ShaderResourceView* buff[4] = { 0 };
		this->dc->PSSetShaderResources(0, 4, buff);
		this->sphereMap.Render(delta, this->mainCam->GetPosition());
	}

	//Pipeline::PipelineManager::Instance().ApplyLightPass(true);

	Pipeline::PipelineManager::Instance().Present();
}

bool LightScatteringScene::Initiate(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	this->device = device;
	this->dc = dc;

	Model bth;
	if (!bth.CreateModel("Models\\bth.obj", device))
		return false;
	this->models.push_back(bth);

	/*Model sphere;
	if (!sphere.CreateModel("Models\\sphere.obj", device))
		return false;
	this->models.push_back(sphere);*/

	this->sphereMap.CreateSkyBox(device, dc);

	return true;
}

void LightScatteringScene::SetMainCamera(Camera* cam)
{
	this->mainCam = cam;
}
