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

	RenderShadowMaps();
	RenderGeometry();
	//RenderSSS();
	RenderLights();
	RenderFinal(delta);

	if(Input::IsKeyDown(VK_UP))			this->player->Forward(+1.5f);
	if(Input::IsKeyDown(VK_DOWN))		this->player->Forward(-1.5f);
	if(Input::IsKeyDown(VK_RIGHT))		this->player->Right(+1.5f);
	if(Input::IsKeyDown(VK_LEFT))		this->player->Right(-1.5f);
	
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

	//Model *bun = new Model();
	//if (!bun->CreateModel("Models\\sbun.obj_low", device))
	//	return false;
	//this->models.push_back(bun);
	//bun->SetScale(0.3f);
	//bun->SetPosition(0.0f, -112.0f, 150.0f);
	//this->player = bun;

	//Model *bud = new Model();
	//if (!bud->CreateModel("Models\\bu.obj", device))
	//	return false;
	//this->models.push_back(bud);
	////bud->SetScale(0.2f);
	//bud->SetPosition(0.0f, -80.0f, 150.0f);
	//this->player = bud;

	//Model *cube = new Model();
	//if (!cube->CreateModel("Models\\plane.obj", device))
	//	return false;
	//cube->SetPosition(0, -100, 50);
	//this->models.push_back(cube);
	//this->player = cube;

	Model *wall = new Model();
	if (!wall->CreateModel("Models\\wall.obj", device))
		return false;
	wall->SetPosition(0, -100, 50);
	this->models.push_back(wall);
	this->player = wall;

	//this->sphereMap.CreateSkyBox(device, dc);

	(this->ground = new Plane())->CreatePlane(device, DirectX::XMFLOAT3(0.0, -100.0f, 0.0f), L"Models\\gray.dds", 1000.0f, 1000.0f, 1.0f);
	this->models.push_back(this->ground);

	if(!CreateLights())
		return false;
	
	TextRender::Initiate(this->device, this->deviceContext, L"Models\\arial.spritefont");

	return true;
}
bool Renderer::CreateLights()
{
	//BasicLightData::PointLight pLight;
	//size_t totPoint = 5;
	//for (size_t i = 0; i < totPoint; i++)
	//{
	//	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	//	pLight.lightColour = DirectX::XMFLOAT3(r, g, b);
	//	pLight.positionRange = DirectX::XMFLOAT4((-((float)totPoint / 2.0f) * 5) + (float)i * 5, -90.0f, -10.0f, 190.0f);
	//	this->pointLights.push_back(pLight);
	//}
	//

#pragma region Create normal lights

	//BasicLightData::Directional dl;
	//dl.color = DirectX::XMFLOAT3(0.8f, 0.8f, 0.8f);
	////dl.direction = DirectX::XMFLOAT3(0.22f, -0.71f, 0.35f);
	//dl.direction = DirectX::XMFLOAT3(0.22f, -0.71f, 0.35f);
	//this->directionalLight.Push(dl);
	//
	//dl.color = DirectX::XMFLOAT3(0.8f, 0.4f, 0.8f);
	//dl.direction = DirectX::XMFLOAT3(0.9f, -0.9f, 0.9f);
	//this->directionalLight.Push(dl);

#pragma endregion

#pragma region Create shadow and lights

	//BasicLightData::ShadowMapLight lds;
	//SimpleMath::Matrix lproj = SimpleMath::Matrix::CreateOrthographic((float)this->desc.width, (float)this->desc.height, this->mainCam->GetNearZ(), this->mainCam->GetFarZ());
	////SimpleMath::Matrix lproj = DirectX::XMLoadFloat4x4(& this->mainCam->GetProjectionMatrix() );
	//SimpleMath::Matrix lview = SimpleMath::Matrix::Identity();
	//if(!lds.shadowMap.Create(this->device, this->deviceContext, this->desc.width, this->desc.height))
	//	return false;
	//lds.attenuation		= 1.0f / 128.0f;
	//lds.bias			= -0.01f;
	//lds.color			= this->directionalLight[0].color;
	//lds.direction		= this->directionalLight[0].direction;
	//lds.falloffStart	= cos(0.5f * (45.0f * DirectX::XM_PI / 180.f));
	//lds.falloffWidth	= 0.05f;
	//lds.farPlane		= 10.0f;
	//lds.position		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	//lds.view			= lview;
	//lds.projection		= lproj;
	//this->shadowMaps.Push(lds);
	//
	////Light casts shadow and sss
	//BasicLightData::ShadowMapLight lds1;
	////lds1.projection = SimpleMath::Matrix::CreateOrthographic((float)this->desc.width, (float)this->desc.height, 0.1f, 1000.0f);
	//lproj = DirectX::XMLoadFloat4x4(& this->mainCam->GetProjectionMatrix() );
	//lview = SimpleMath::Matrix::Identity();
	//if(!lds1.shadowMap.Create(this->device, this->deviceContext, this->desc.width, this->desc.height))
	//	return false;
	//lds1.attenuation	= 1.0f / 128.0f;
	//lds1.bias			= -0.01f;
	//lds1.color			= this->directionalLight[1].color;
	//lds1.direction		= this->directionalLight[1].direction;
	//lds1.falloffStart	= cos(0.5f * (45.0f * DirectX::XM_PI / 180.f));
	//lds1.falloffWidth	= 0.05f;
	//lds1.farPlane		= 10.0f;
	//lds1.position		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	//lds1.view			= lview;
	//lds1.projection		= lproj;
	//this->shadowMaps.Push(lds1);

#pragma endregion
	{
		//Light casts shadow and sss
		BasicLightData::ShadowMapLight lds1;
		{
			if(!lds1.shadowMap.Create(this->device, this->deviceContext, this->desc.width, this->desc.height))
				return false;
		
			lds1.attenuation		= SimpleMath::Vector3(0.4f, 0.02f, 0.0f);
			lds1.color				= SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
			lds1.cone				= 50.0f;
			lds1.range				= 1000.0f;
		
			lds1.camera.SetProjectionMatrix(DirectX::XMConvertToRadians(45.0f), ((float)this->desc.width/(float)this->desc.height), this->mainCam->GetNearZ(), this->mainCam->GetFarZ());
			lds1.camera.SetPosition(0.0f, 100.0f, -30.0f);
			lds1.camera.SetRotation(40.0f, 0.0f, 0.0f);
			lds1.camera.Render();
		}
		
		BasicLightData::ShadowMapLight lds2;
		{
			if(!lds2.shadowMap.Create(this->device, this->deviceContext, 2048, 2048))
				return false;
		
			lds2.attenuation		= SimpleMath::Vector3(0.2f, 0.001f, 0.0001f);
			lds2.color				= SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
			lds2.cone				= 20.0f;
			lds2.range				= 1000.0f;
		
			lds2.camera.SetProjectionMatrix(DirectX::XMConvertToRadians(45.0f), (2048.0f/2048.0f), this->mainCam->GetNearZ(), this->mainCam->GetFarZ());
			lds2.camera.SetPosition(0.0f, -90.0f, 0.0f);
			lds2.camera.Render();
		}
		/*
		BasicLightData::ShadowMapLight lds3;
		{
			if(!lds3.shadowMap.Create(this->device, this->deviceContext, 2048, 2048))
				return false;
		
			lds3.attenuation		= SimpleMath::Vector3(0.4f, 0.02f, 0.0f);
			lds3.color				= SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
			lds3.cone				= 200.0f;
			lds3.range				= 1000.0f;
		
			lds3.camera.SetProjectionMatrix(DirectX::XMConvertToRadians(45.0f), (2048.0f/2048.0f), this->mainCam->GetNearZ(), this->mainCam->GetFarZ());
			lds3.camera.SetPosition(20.0f, 20.0f, 0.0f);
			lds3.camera.SetRotation(45.0f, -90.0f, 0.0f);
			lds3.camera.Render();
		}
		BasicLightData::ShadowMapLight lds4;
		{
			if(!lds4.shadowMap.Create(this->device, this->deviceContext, 2048, 2048))
				return false;
		
			lds4.attenuation		= SimpleMath::Vector3(0.4f, 0.02f, 0.0f);
			lds4.color				= SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
			lds4.cone				= 500.0f;
			lds4.range				= 1000.0f;
		
			lds4.camera.SetProjectionMatrix(DirectX::XMConvertToRadians(45.0f), (2048.0f/2048.0f), this->mainCam->GetNearZ(), this->mainCam->GetFarZ());
			lds4.camera.SetPosition(-20.0f, 20.0f, 0.0f);
			lds4.camera.SetRotation(45.0f, 90.0f, 0.0f);
			lds4.camera.Render();
		}
		BasicLightData::ShadowMapLight lds5;
		{
			if(!lds5.shadowMap.Create(this->device, this->deviceContext, 2048, 2048))
				return false;
		
			lds5.attenuation		= SimpleMath::Vector3(0.4f, 0.02f, 0.0f);
			lds5.color				= SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
			lds5.cone				= 500.0f;
			lds5.range				= 1000.0f;
		
			lds5.camera.SetProjectionMatrix(DirectX::XMConvertToRadians(45.0f), (2048.0f/2048.0f), this->mainCam->GetNearZ(), this->mainCam->GetFarZ());
			lds5.camera.SetPosition(0.0f, 20.0f, -25.0f);
			lds5.camera.SetRotation(45.0f, 0.0f, 0.0f);
			lds5.camera.Render();
		}
		*/
		this->shadowMaps.Push(lds1);
		this->shadowMaps.Push(lds2);
		//this->shadowMaps.Push(lds3);
		//this->shadowMaps.Push(lds4);
		//this->shadowMaps.Push(lds5);
		

		//SimpleMath::Matrix lproj = SimpleMath::Matrix::CreateOrthographic((float)this->desc.width, (float)this->desc.height, this->mainCam->GetNearZ(), this->mainCam->GetFarZ());
		//SimpleMath::Matrix lview = SimpleMath::Matrix::CreateWorld(SimpleMath::Vector3(0.0f, 0.0f, -55.0f), SimpleMath::Vector3(0.0f, 0.0f, 1.0f), SimpleMath::Vector3(0.0f, 1.0f, 0.0f));

		//size_t totSpot = 1;
		//BasicLightData::Spotlight sl;
		//for (size_t i = 0; i < totSpot; i++)
		//{
		//	sl.coneAngle = 20.0f;
		//	sl.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
		//	sl.position = DirectX::XMFLOAT3(0.0f, 0.0f, -10.0f);
		//	sl.range = 1000.0f;
		//	sl.unitDir = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
		//	sl.attenuation = DirectX::XMFLOAT3(0.4f, 0.02f, 0.0f);
		//	this->spotLight.push_back(sl);
		//}
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
			//this->shadowMaps[i].view = XMLoadFloat4x4(&this->mainCam->GetViewMatrix());
			//this->shadowMaps[i].projection = XMLoadFloat4x4(&this->mainCam->GetProjectionMatrix());

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

	//if (this->directionalLight.Size() > 0)
	//{
	//	Pipeline::PipelineManager::Instance().ApplyDepthPass();
	//
	//	for (size_t i = 0; i < this->directionalLight.Size(); i++)
	//	{
	//		//Pipeline::PipelineManager::Instance().RenderDepthMap(DirectX::XMFLOAT3(0.0f, 10.0f, 0.0f), this->directionalLight[i].direction);
	//
	//		DirectX::XMFLOAT4X4 m;
	//		DirectX::XMFLOAT3 f(0.2f, 0.3f, 1.0f);
	//		XMStoreFloat4x4(&m, DirectX::XMMatrixRotationNormal(DirectX::XMLoadFloat3(&f),0.7f));
	//
	//		//Pipeline::PipelineManager::Instance().SetSceneMatrixBuffers(Pipeline::PipelineManager::Instance().GetDepthCameraView(),
	//		//															Pipeline::PipelineManager::Instance().GetDepthCameraProj());
	//	   
	//		for (size_t i = 0; i < this->models.size(); i++)
	//		{
	//			UINT off = 0;
	//			this->deviceContext->IASetVertexBuffers(0, 1, &this->models[i].GetMesh().vertexBuffer, &this->models[i].GetMesh().vertexStride, &off);
	//			Pipeline::PipelineManager::Instance().SetObjectMatrixBuffers(m, this->models[i].GetWorldInversTranspose());
	//			this->deviceContext->Draw(this->models[i].GetMesh().vertexCount, 0);
	//		}
	//
	//		this->ground.RenderForDepthMap(this->deviceContext);
	//	}
	//}
	
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
	clock.reset();

	Pipeline::LightPass::LightData lData;
	memset(&lData, 0, sizeof(Pipeline::LightPass::LightData));

	if (this->pointLights.Size())		lData.pointData = &this->pointLights[0];
	lData.pointCount = (int)this->pointLights.Size();

	if (this->spotLight.Size())			lData.spotData = &this->spotLight[0];
	lData.spotCount = this->spotLight.Size();

	if (this->directionalLight.Size())	lData.dirData = &this->directionalLight[0];
	lData.dirCount = (int)this->directionalLight.Size();

	if (this->shadowMaps.Size())	lData.shadowData = &this->shadowMaps[0];
	lData.shadowCount = this->shadowMaps.Size();

	lData.invProj = this->mainCam->GetInverseProjectionMatrix();
	lData.view = this->mainCam->GetViewMatrix();
	lData.ambientLight = DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f);

	Pipeline::PipelineManager::Instance().ApplyLightPass(lData);

	float tid = (float)clock.getElapsedSeconds();
	if(time > timeMax)
	{
		lightPassStrign.resize(0); char a[50];
		/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); lightPassStrign.append("Light pass - FPS: "); lightPassStrign.append(a);
		/*/DELTA TIME */	sprintf_s(a, "%f", tid); lightPassStrign.append(" - ms:"); lightPassStrign.append(a);
	}
}
void Renderer::RenderSSS()
{
	if (this->shadowMaps.Size() > 0)
	{
		for (size_t i = 0; i < this->shadowMaps.Size(); i++)
		{
			//Pipeline::PipelineManager::Instance().ApplySSSPass(
		}
		//Pipeline::PipelineManager::Instance().ApplyTranslucentShadowMap();

		//for (size_t i = 0; i < this->directionalLight.Size(); i++)
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
	}
}
void Renderer::RenderFinal(float dt)
{
	clock.reset();
	{
		PrintStats(dt);
		Pipeline::PipelineManager::Instance().ApplyFinalPass();
	}

	float tid = (float)clock.getElapsedSeconds();
	if(time > timeMax)
	{
		finalPassString.resize(0); char a[50];
		/*/FPS*/			sprintf_s(a, "%d", (int)(1.0f / tid)); finalPassString.append("Final pass - FPS: "); finalPassString.append(a);
		/*/DELTA TIME */	sprintf_s(a, "%f", tid); finalPassString.append(" - ms:"); finalPassString.append(a);
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
	TextRender::Write(Util::StringToWstring(lightPassStrign, std::wstring()).c_str(), 10, 250);
	TextRender::Write(Util::StringToWstring(depthPassString, std::wstring()).c_str(), 10, 270);
	TextRender::Write(Util::StringToWstring(finalPassString, std::wstring()).c_str(), 10, 290);
}


