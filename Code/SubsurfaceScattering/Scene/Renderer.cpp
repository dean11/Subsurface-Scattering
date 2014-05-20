#include "Renderer.h"
#include "..\Utilities\DepthCamera.h"
#include "..\Input.h"
#include "..\Utilities\TextRender.h"


static float time = 0.0f;
static float timeMax = 0.5f;
std::string geomPassStrign = "Geometry pass: ";
std::string lightPassStrign = "Light pass: ";
std::string depthPassString = "Depth pass: ";
std::string finalPassString = "Final pass: ";
std::string postPassStrign = "Post stats: ";
std::string totalStatsString = "Total stats: ";
bool moveObjectToggle = false;

Renderer::Renderer()
{
}
Renderer::~Renderer()
{
	
}

void Renderer::Release()
{
	for (size_t i = 0; i < this->models.size(); i++)
	{
		this->models[i]->Release();
		delete this->models[i];
		this->models[i] = 0;;
	}

	for (size_t i = 0; i < this->shadowMaps.Size(); i++)
	{
		this->shadowMaps[i].shadowMap.Release();
	}

	this->device->Release();
	this->deviceContext->Release();
}
void Renderer::Frame(float delta)
{
	time += delta;

	if(Input::IsKeyDown(VK_UP))			this->player->Forward(+1.0f);
	if(Input::IsKeyDown(VK_DOWN))		this->player->Forward(-1.0f);
	if(Input::IsKeyDown(VK_RIGHT))		this->player->Right(+1.0f);
	if(Input::IsKeyDown(VK_LEFT))		this->player->Right(-1.0f);

	if(Input::IsKeyDown(VK_0) && this->shadowMaps.Size() > 0) this->shadowMaps[0].isOn = Input::IsKeyDown(VK_MENU) ? true : false;
	if(Input::IsKeyDown(VK_1) && this->shadowMaps.Size() > 1) this->shadowMaps[1].isOn = Input::IsKeyDown(VK_MENU) ? true : false;
	if(Input::IsKeyDown(VK_2) && this->shadowMaps.Size() > 2) this->shadowMaps[2].isOn = Input::IsKeyDown(VK_MENU) ? true : false;
	if(Input::IsKeyDown(VK_3) && this->shadowMaps.Size() > 3) this->shadowMaps[3].isOn = Input::IsKeyDown(VK_MENU) ? true : false;
	if(Input::IsKeyDown(VK_4) && this->shadowMaps.Size() > 4) this->shadowMaps[4].isOn = Input::IsKeyDown(VK_MENU) ? true : false;
	if(Input::IsKeyDown(VK_5) && this->shadowMaps.Size() > 5) this->shadowMaps[5].isOn = Input::IsKeyDown(VK_MENU) ? true : false;
	if(Input::IsKeyDown(VK_6) && this->shadowMaps.Size() > 6) this->shadowMaps[6].isOn = Input::IsKeyDown(VK_MENU) ? true : false;
	if(Input::IsKeyDown(VK_7) && this->shadowMaps.Size() > 7) this->shadowMaps[7].isOn = Input::IsKeyDown(VK_MENU) ? true : false;
	if(Input::IsKeyDown(VK_8) && this->shadowMaps.Size() > 8) this->shadowMaps[8].isOn = Input::IsKeyDown(VK_MENU) ? true : false;
	if(Input::IsKeyDown(VK_9) && this->shadowMaps.Size() > 9) this->shadowMaps[9].isOn = Input::IsKeyDown(VK_MENU) ? true : false;

	RenderShadowMaps();
	RenderGeometry();
	RenderPostPass(delta);
	
	PrintStats(delta);
	//ID3D11ShaderResourceView* vb[20] = {0};
	//for (size_t i = 0; i < this->shadowMaps.Size(); i++)
	//{
	//	vb[i] = this->shadowMaps[i].shadowMap;
	//}
	//PipelineManager::Instance().ApplyUIPass(vb, shadowMaps.Size());
	PipelineManager::Instance().ApplyUIPass();
	PipelineManager::Instance().Present();
	
	if (time > timeMax)
		time = 0.0f;
}

bool Renderer::Initiate(RendererInitDesc& desc)
{
	this->desc = desc;
	if(!CreateDeviceAndContext()) 
		return false;

	if (!Pipeline::PipelineManager::Instance().Initiate(this->device, this->deviceContext, (int)this->desc.width, (int)this->desc.height))
		return false;

	//Model *bth = new Model();
	//if (!bth->CreateModel("Models\\bth.righthanded.obj", device))
	//	return false;
	//this->models.push_back(bth);
	//
	for (size_t i = 0; i < 1; i++)
	{
		Model *bun = new Model();
		if (!bun->CreateModel("Models\\sbun.obj", device))
		//if (!bun->CreateModel("Models\\bunny.obj", device))
			return false;
		this->models.push_back(bun);
		bun->SetScale(0.3f);
		bun->SetPosition(40.0f * i, -120.0f, 0.0f);
		//bun->SetPosition(10.0f * i, -112.0f, 150.0f);
		this->player = bun;
	}
	//
	//Model *bud = new Model();
	//if (!bud->CreateModel("Models\\bu.obj", device))
	//	return false;
	//this->models.push_back(bud);
	//bud->SetPosition(-30.0f, -80.0f, 150.0f);
	//this->player = bud;

	//Model *cube = new Model();
	//if (!cube->CreateModel("Models\\plane.obj", device))
	//	return false;
	//cube->SetPosition(0, -100, 50);
	//this->models.push_back(cube);
	//this->player = cube;

	//Model *wall = new Model();
	//if (!wall->CreateModel("Models\\wall.obj", device))
	//	return false;
	//wall->SetPosition(0, -100, 80);
	////wall->Rotate(DirectX::SimpleMath::Vector3(10.0f, 0.0f, 0.0f));
	//this->models.push_back(wall);
	//this->player = wall;
	//
	//Model *wallSmall = new Model();
	//if (!wallSmall->CreateModel("Models\\wallSmall.obj", device))
	//	return false;
	//wallSmall->SetPosition(0, -100, 0);
	////wallSmall->SetScale(1.0f, 1.0f, 0.3f);
	////wallSmall->Rotate(DirectX::SimpleMath::Vector3(10.0f, 0.0f, 0.0f));
	//this->models.push_back(wallSmall);
	//this->player = wallSmall;

	//this->sphereMap.CreateSkyBox(device, dc);

	(this->ground = new Plane())->CreatePlane(device, DirectX::XMFLOAT3(0.0, -100.0f, 0.0f), L"Models\\gray.dds", 1000.0f, 1000.0f, 1.0f);
	this->models.push_back(this->ground);

	if(!CreateLights())
		return false;
	
	TextRender::Initiate(this->device, this->deviceContext, L"Models\\arial.spritefont");

	return true;
}

struct pr
{
	XMFLOAT3 p;
	XMFLOAT3 r;
	pr(XMFLOAT3 _p, XMFLOAT3 _r)
	{
		p = _p;
		r = _r;
	}
};
bool Renderer::CreateLights()
{
#pragma region Create normal lights

	//BasicLightData::PointLight pLight;
	//size_t totPoint = 15;
	//for (size_t i = 0; i < totPoint; i++)
	//{
	//	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	pLight.lightColour = DirectX::XMFLOAT3(r, g, b);
	//	pLight.positionRange = DirectX::XMFLOAT4((-((float)totPoint / 2.0f) * 40) + (float)i * 40, -90.0f, -10.0f, 190.0f);
	//	this->pointLights.Push(pLight);
	//}
	
	//BasicLightData::Directional dl;
	//dl.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	//dl.direction = DirectX::XMFLOAT3(0.0f, -0.7f, -0.3f);
	//dl.intensity = 0.2f;
	//this->directionalLight.Push(dl);
	//
	//dl.color = DirectX::XMFLOAT3(0.8f, 0.4f, 0.8f);
	//dl.direction = DirectX::XMFLOAT3(0.9f, -0.9f, 0.9f);
	//this->directionalLight.Push(dl);

#pragma endregion

#pragma region Create shadow and lights
	{
		const float spot = 26.0f;
		const float range = 450.0f;
		const SimpleMath::Vector3 att(0.1f, 0.0f, 0.00002f);
		
		std::vector<pr> s;
		//s.push_back (pr(XMFLOAT3(+100.0f, -90.0f, 0.0f) ,XMFLOAT3(0.0f, -90.0f, 0.0f)));
		//s.push_back (pr(XMFLOAT3(-100.0f, -90.0f, 0.0f) ,XMFLOAT3(0.0f, 90.0f, 0.0f)));
		s.push_back (pr(XMFLOAT3(0.0f, -90.0f, -100.0f) ,XMFLOAT3(0.0f, 0.0f, 0.0f)));
		//s.push_back (pr(XMFLOAT3(0.0f, -90.0f, 100.0f) ,XMFLOAT3(0.0f, 180.0f, 0.0f)));
		//s.push_back (pr(XMFLOAT3(-20.0f, -50.0f, 0.0f) ,XMFLOAT3(90.0f, 0.0f, 0.0f)));
		//s.push_back (pr(XMFLOAT3(0.0f, -90.0f, 0.0f) ,XMFLOAT3(0.0f, 0.0f, 0.0f)));
		//s.push_back (pr(XMFLOAT3(20.0f, -50.0f, 0.0f) ,XMFLOAT3(90.0f, 0.0f, 0.0f)));
		//s.push_back (pr(XMFLOAT3(0.0f, -40.0f, 0.0f) ,XMFLOAT3(90.0f, 0.0f, 0.0f)));
		//s.push_back (pr(XMFLOAT3(0.0f, -40.0f, 0.0f) ,XMFLOAT3(90.0f, 0.0f, 0.0f)));
		//s.push_back (pr(XMFLOAT3(0.0f, -40.0f, 0.0f) ,XMFLOAT3(90.0f, 0.0f, 0.0f)));

		//Light casts shadow and sss
		for (size_t i = 0; i < s.size(); i++)
		{
			BasicLightData::ShadowMapLight shadow;
			if(!shadow.shadowMap.Create(this->device, this->deviceContext, this->desc.width, this->desc.height))
				break;

			shadow.color.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			shadow.color.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			shadow.color.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

			shadow.attenuation		= att;
			shadow.spot				= spot;
			shadow.range			= range;
		
			shadow.camera.SetProjectionMatrix(DirectX::XMConvertToRadians(65.0f), ((float)this->desc.width/(float)this->desc.height), 0.1, range);
			shadow.camera.SetPosition(s[i].p);
			//shadow.camera.SetRotation(s[i].r);
			shadow.camera.Render();
			shadow.isOn = true;
			this->shadowMaps.Push(shadow);
		}

#pragma endregion
	}
	return true;
}
bool Renderer::CreateDeviceAndContext()
{
	::D3D_DRIVER_TYPE driverType = ::D3D_DRIVER_TYPE_HARDWARE;
		
	UINT createDeviceFlags = ::D3D11_CREATE_DEVICE_SINGLETHREADED;
	#if defined(DEBUG) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL featureLevelsToTry[] = 
	{ D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_11_1 };
	D3D_FEATURE_LEVEL initiatedFeatureLevel;
	HRESULT hr = S_OK;
	if( FAILED( hr = ::D3D11CreateDevice(	NULL, // default adapter
											driverType,
											NULL, // no software device
											createDeviceFlags,
											featureLevelsToTry, Util::NumElementsOf(featureLevelsToTry),
											D3D11_SDK_VERSION,
											&this->device,
											&initiatedFeatureLevel,
											&this->deviceContext ) ) )
	{ // if failed
		return false;
	}

	return true;
}
void Renderer::SetMainCamera(Camera* cam)
{
	this->mainCam = cam;
}


void Renderer::RenderShadowMaps()
{
	clock.reset();
	{
		//Render scene from each view
		for (size_t i = 0; i < this->shadowMaps.Size(); i++)
		{
			if(!this->shadowMaps[i].isOn) continue;

			this->shadowMaps[i].shadowMap.Begin(XMLoadFloat4x4(&this->shadowMaps[i].camera.GetViewMatrix()), XMLoadFloat4x4(&this->shadowMaps[i].camera.GetProjectionMatrix()));
			{
				for (size_t k = 0; k < this->models.size(); k++)
				{
					this->shadowMaps[i].shadowMap.Draw(*this->models[k]);
				}

			} this->shadowMaps[i].shadowMap.End();
		}
	}
	float tid = (float)clock.getElapsedSeconds();
	if(time > timeMax)
	{
		depthPassString.resize(0); char a[50];
		/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); depthPassString.append("Depth pass - FPS: "); depthPassString.append(a);
		/*/DELTA TIME */	sprintf_s(a, "%f", tid); depthPassString.append(" - ms:"); depthPassString.append(a);
	}
	
}
void Renderer::RenderGeometry()
{
	clock.reset();

	Pipeline::PipelineManager::Instance().ApplyGeometryPass();
	{
		Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(this->mainCam->GetViewMatrix(), this->mainCam->GetProjectionMatrix());
		
		for (size_t i = 0; i < this->models.size(); i++)
		{
			//if(i == 0)
			//	this->models[i].Rotate(SimpleMath::Vector3(0.003f, 0.003f, 0.003f));

			this->models[i]->DrawModel(this->deviceContext);
		}
	}

	float tid = (float)clock.getElapsedSeconds();
	if(time > timeMax)
	{
		geomPassStrign.resize(0); char a[50];
		/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); geomPassStrign.append("Geometry pass - FPS: "); geomPassStrign.append(a);
		/*/DELTA TIME */	sprintf_s(a, "%f", tid); geomPassStrign.append(" - ms:"); geomPassStrign.append(a);
	}
}
void Renderer::RenderLights()
{
	//clock.reset();
	//
	//Pipeline::LightPass::LightData lData;
	//memset(&lData, 0, sizeof(Pipeline::LightPass::LightData));
	//
	//if (this->pointLights.Size())		lData.pointData = &this->pointLights[0];
	//lData.pointCount = (int)this->pointLights.Size();
	//
	//if (this->spotLight.Size())			lData.spotData = &this->spotLight[0];
	//lData.spotCount = this->spotLight.Size();
	//
	//if (this->directionalLight.Size())	lData.dirData = &this->directionalLight[0];
	//lData.dirCount = (int)this->directionalLight.Size();
	//
	//if (this->shadowMaps.Size())	lData.shadowData = this->shadowMaps;
	//lData.shadowCount = this->shadowMaps.Size();
	//
	//lData.proj = this->mainCam->GetProjectionMatrix();
	//lData.view = this->mainCam->GetViewMatrix();
	//lData.ambientLight = DirectX::XMFLOAT3(0.28f, 0.28f, 0.28f);
	//
	//Pipeline::PipelineManager::Instance().ApplyLightPass(lData);
	//
	//float tid = (float)clock.getElapsedSeconds();
	//if(time > timeMax)
	//{
	//	lightPassStrign.resize(0); char a[50];
	//	/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); lightPassStrign.append("Light pass - FPS: "); lightPassStrign.append(a);
	//	/*/DELTA TIME */	sprintf_s(a, "%f", tid); lightPassStrign.append(" - ms:"); lightPassStrign.append(a);
	//}
}
void Renderer::RenderSSS()
{
	//if (this->shadowMaps.Size() > 0)
	//{
	//	for (size_t i = 0; i < this->shadowMaps.Size(); i++)
	//	{
	//		//Pipeline::PipelineManager::Instance().ApplySSSPass(
	//	}
	//	//Pipeline::PipelineManager::Instance().ApplyTranslucentShadowMap();
	//
	//	//for (size_t i = 0; i < this->directionalLight.Size(); i++)
		//{
		//	Pipeline::PipelineManager::Instance().RenderDepthMap(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f), this->directionalLight[i].direction);
		//
		//	DirectX::XMFLOAT4X4 m;
		//	DirectX::XMFLOAT3 f(0.2f, 0.3f, 1.0f);
		//	XMStoreFloat4x4(&m, DirectX::XMMatrixRotationNormal(DirectX::XMLoadFloat3(&f),0.7f));
		//
		//	Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(Pipeline::PipelineManager::Instance().GetDepthCameraView(),
		//																Pipeline::PipelineManager::Instance().GetDepthCameraProj());
		//
		//	for (size_t i = 0; i < this->models.size(); i++)
		//	{
		//		UINT off = 0;
		//		this->deviceContext->IASetVertexBuffers(0, 1, &this->models[i].GetMesh().vertexBuffer, &this->models[i].GetMesh().vertexStride, &off);
		//		Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(m, this->models[i].GetWorldInversTranspose());
		//		this->deviceContext->Draw(this->models[i].GetMesh().vertexCount, 0);
		//	}
		//
		//	this->ground.RenderForDepthMap(this->deviceContext);
		//}
	//}
}
void Renderer::RenderFinal(float dt)
{
	//clock.reset();
	//{
	//	PrintStats(dt);
	//	Pipeline::PipelineManager::Instance().ApplyFinalPass();
	//}
	//
	//float tid = (float)clock.getElapsedSeconds();
	//if(time > timeMax)
	//{
	//	finalPassString.resize(0); char a[50];
	//	/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); finalPassString.append("Final pass - FPS: "); finalPassString.append(a);
	//	/*/DELTA TIME */	sprintf_s(a, "%f", tid); finalPassString.append(" - ms:"); finalPassString.append(a);
	//}
}
void Renderer::RenderPostPass(float dt)
{
	clock.reset();

	Pipeline::LightPass::LightData lData;
	memset(&lData, 0, sizeof(Pipeline::LightPass::LightData));

	if (this->pointLights.Size())		lData.pointData = &this->pointLights[0];
	lData.pointCount = (int)this->pointLights.Size();

	if (this->spotLight.Size())			lData.spotData = &this->spotLight[0];
	lData.spotCount = this->spotLight.Size();

	if (this->directionalLight.Size())	lData.dirData = &this->directionalLight[0];
	lData.dirCount = (int)this->directionalLight.Size();

	BasicLightData::ShadowMapLight* shadowBuff[20] = {0};
	int k = 0;
	if (this->shadowMaps.Size())	
	{
		for (size_t i = 0; i < this->shadowMaps.Size(); i++)
		{
			if(this->shadowMaps[i].isOn)
			{
				shadowBuff[k] = &this->shadowMaps[i];
				k++;
			}
		}
		lData.shadowData = shadowBuff;
		lData.shadowCount = k;
	}

	lData.proj = this->mainCam->GetProjectionMatrix();
	lData.view = this->mainCam->GetViewMatrix();
	lData.ambientLight = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	lData.cameraPos = this->mainCam->GetForward();

		Pipeline::PipelineManager::Instance().ApplyPostEffectPass(lData);

	float tid = (float)clock.getElapsedSeconds();
	if(time > timeMax)
	{
		postPassStrign.resize(0); char a[50];
		/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); postPassStrign.append("PostEffect pass - FPS: "); postPassStrign.append(a);
		/*/DELTA TIME */	sprintf_s(a, "%f", tid); postPassStrign.append(" - ms:"); postPassStrign.append(a);
	}
}

void Renderer::PrintStats(float dt)
{
	if (time > timeMax)
	{
		totalStatsString.resize(0);
		totalStatsString = "Total ";
		char a[50];
		//FPS
		sprintf_s(a, "%d", (int)(1.0f / dt));
		totalStatsString.append("FPS: ");
		totalStatsString.append(a);
		//DELTA TIME
		sprintf_s(a, "%f", dt);
		totalStatsString.append(" - ms:");
		totalStatsString.append(a);
	}

	TextRender::Write(Util::StringToWstring(totalStatsString, std::wstring()).c_str(), 10, 210);
	TextRender::Write(Util::StringToWstring(geomPassStrign, std::wstring()).c_str(), 10, 230);
	TextRender::Write(Util::StringToWstring(depthPassString, std::wstring()).c_str(), 10, 250);
	TextRender::Write(Util::StringToWstring(postPassStrign, std::wstring()).c_str(), 10, 270);
	
}


