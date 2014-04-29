#include "LightScatteringScene.h"
#include "..\Utilities\BasicLightData.h"

LightScatteringScene::LightScatteringScene(LSSInitDesc& desc)
{
	this->desc = desc;
	this->sphereMap = SphereMap();
	//this->pointLight = PointLight();
}

LightScatteringScene::~LightScatteringScene()
{

}

void LightScatteringScene::Frame(float delta)
{
	Pipeline::PipelineManager::Instance().ApplyGeometryPass();
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
	Pipeline::PipelineManager::LightData lData;
	lData.pointData = &this->data;
	lData.pointCount = 1;
	Pipeline::PipelineManager::Instance().ApplyLightPass(lData);


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

	if (!this->sphereMap.CreateSkyBox(device, dc)) return false;

	//BasicLightData::PointLight data;
	data.attenuation			= DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	data.lightColour			= DirectX::XMFLOAT4(0.7f, 1.0f, 0.4f, 0.0f);
	//data.lightColour.ambient	= DirectX::XMFLOAT4(0.2f, 0.2f, 0.2, 0.0f);
	//data.lightColour.diffuse	= DirectX::XMFLOAT4(0.7f, 1.0f, 0.4, 0.0f);
	//data.lightColour.specular	= DirectX::XMFLOAT4(0.2f, 0.4f, 0.3, 0.0f);
	data.positionRange			= DirectX::XMFLOAT4(0.0f, 10.0f, 10.0f, 20.0f);
	//if (!this->pointLight.CreatePointLight("Models\\sphereTriLow.obj", data, device)) return false;

	return true;
}

void LightScatteringScene::SetMainCamera(Camera* cam)
{
	this->mainCam = cam;
}
