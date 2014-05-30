#include "Renderer.h"
#include "..\Utilities\DepthCamera.h"
#include "..\Input.h"
#include "..\Utilities\TextRender.h"
#include "..\Pipeline\Material.h"

float speed = 45.0f;
static float time = 0.0f;
static float timeMax = 0.5f;
std::string totalStatsString = "Total stats: ";
std::string ShadowStatsString = "Shadow pass: ";
std::string MainStatsString = "Main pass: ";
std::string PostStatsString = "Post pass: ";
bool moveObjectToggle = false;
ID3D11Counter* d3dcounter = 0;

enum GTS
{
	GTS_BeginFrame,
	GTS_ShadowObjects,
	GTS_MainObjects,
	GTS_PostPass,
	GTS_EndFrame,

	GTS_Max
};
ID3D11Query * m_apQueryTsDisjoint[2];		// "Timestamp disjoint" query; records whether timestamps are valid
ID3D11Query * m_apQueryTs[GTS_Max][2];
int m_iFrameQuery = 0;
int m_iFrameCollect = -1;
float m_adT[GTS_Max] = {0};						// Last frame's timings (each relative to previous GTS)
float m_adTAvg[GTS_Max] =  {0};					// Timings averaged over 0.5 second

float m_adTTotalAvg[GTS_Max] = {0};				// Total timings thus far within this averaging period
int m_frameCountAvg = 0;						// Frames rendered in current averaging period
float m_tBeginAvg = 0;							// Time at which current averaging period started
#define FPS60	1.0f/60.0f
void Renderer::WaitForDataAndUpdate ()
{
	if (m_iFrameCollect < 0)
	{
		// Haven't run enough frames yet to have data
		m_iFrameCollect = 0;
		return;
	}

	// Wait for data
	while (this->deviceContext->GetData(m_apQueryTsDisjoint[m_iFrameCollect], NULL, 0, 0) == S_FALSE)
	{
		Sleep(1);
	}

	int iFrame = m_iFrameCollect;
	++m_iFrameCollect &= 1;

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestampDisjoint;
	if (this->deviceContext->GetData(m_apQueryTsDisjoint[iFrame], &timestampDisjoint, sizeof(timestampDisjoint), 0) != S_OK)
		return;

	if (timestampDisjoint.Disjoint)
		return;

	UINT64 timestampPrev;
	if (this->deviceContext->GetData(m_apQueryTs[GTS_BeginFrame][iFrame], &timestampPrev, sizeof(UINT64), 0) != S_OK)
		return;

	for (GTS gts = GTS(GTS_BeginFrame + 1); gts < GTS_Max; gts = GTS(gts + 1))
	{
		UINT64 timestamp;
		if (this->deviceContext->GetData(m_apQueryTs[gts][iFrame], &timestamp, sizeof(UINT64), 0) != S_OK)
			return;

		m_adT[gts] = float(timestamp - timestampPrev) / float(timestampDisjoint.Frequency);
		timestampPrev = timestamp;

		m_adTTotalAvg[gts] += m_adT[gts];
	}

	++m_frameCountAvg;
	if (this->clock.getElapsedSeconds() > m_tBeginAvg + 0.5f)
	{
		for (GTS gts = GTS_BeginFrame; gts < GTS_Max; gts = GTS(gts + 1))
		{
			m_adTAvg[gts] = m_adTTotalAvg[gts] / m_frameCountAvg;
			m_adTTotalAvg[gts] = 0.0f;
		}

		m_frameCountAvg = 0;
		m_tBeginAvg = (float)this->clock.getElapsedSeconds();
	}
}

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
	Util::SAFE_RELEASE(d3dcounter);
	this->device->Release();
	this->deviceContext->Release();
}
void Renderer::Frame(float delta)
{
	time += delta;
	static float alpha = 0;

	alpha += delta;
	if(alpha < FPS60) return;
	alpha = 0.0f;

	this->deviceContext->Begin(m_apQueryTsDisjoint[m_iFrameQuery]);
	this->deviceContext->End(m_apQueryTs[GTS_BeginFrame][m_iFrameQuery]);
	{ 

		if(Input::IsKeyDown(VK_UP))			this->player->Forward(+speed);
		if(Input::IsKeyDown(VK_DOWN))		this->player->Forward(-speed);
		if(Input::IsKeyDown(VK_RIGHT))		this->player->Right(+speed);
		if(Input::IsKeyDown(VK_LEFT))		this->player->Right(-speed);
		if(Input::IsKeyDown(VK_ADD))		speed += 0.8f;
		if(Input::IsKeyDown(VK_SUBTRACT))	speed -= 0.8f;

		if(Input::IsKeyDown(VK_R))			this->ground->ToggleVisibility();

		if(Input::IsKeyDown(VK_Y))			this->shadowMaps[2].camera.RelativeForward(+2.0f);
		if(Input::IsKeyDown(VK_H))			this->shadowMaps[2].camera.RelativeForward(-2.0f);

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

		if( Input::IsKeyDown(49 /*1*/) ) { this->player = this->models.size() > 0 ? this->models[0] : this->player; speed = 2.0f; }
		if( Input::IsKeyDown(50 /*2*/) ) { this->player = this->models.size() > 1 ? this->models[1] : this->player; speed = 2.0f; }
		if( Input::IsKeyDown(51 /*3*/) ) { this->player = this->models.size() > 2 ? this->models[2] : this->player; speed = 2.0f; }
		if( Input::IsKeyDown(52 /*4*/) ) { this->player = this->models.size() > 3 ? this->models[3] : this->player; speed = 2.0f; }
		if( Input::IsKeyDown(53 /*5*/) ) { this->player = this->models.size() > 4 ? this->models[4] : this->player; speed = 2.0f; }
		if( Input::IsKeyDown(54 /*6*/) ) { this->player = this->models.size() > 5 ? this->models[5] : this->player; speed = 2.0f; }

		if( Input::IsKeyDown(55 /*7*/) ) { this->player = this->models.size() > 6 ? this->models[6] : this->player; speed = 2.0f; }
		if( Input::IsKeyDown(56 /*8*/) ) { this->player = this->models.size() > 7 ? this->models[7] : this->player; speed = 2.0f; }
		if( Input::IsKeyDown(57 /*9*/) ) { this->player = this->models.size() > 8 ? this->models[8] : this->player; speed = 2.0f; }
		if( Input::IsKeyDown(48 /*0*/) ) { this->player = this->models.size() > 9 ? this->models[9] : this->player; speed = 2.0f; }

		RenderShadowMaps();
		this->deviceContext->End(m_apQueryTs[GTS_ShadowObjects][m_iFrameQuery]);

		RenderGeometry();
		this->deviceContext->End(m_apQueryTs[GTS_MainObjects][m_iFrameQuery]);

		RenderPostPass(delta);
		this->deviceContext->End(m_apQueryTs[GTS_PostPass][m_iFrameQuery]);

	} WaitForDataAndUpdate();

	float dTDrawTotal = 0.0f;
	for (GTS gts = GTS_BeginFrame; gts < GTS_EndFrame; gts = GTS(gts + 1))
		dTDrawTotal += m_adTAvg[gts];

	char a[50];
	totalStatsString.resize(0);
	ShadowStatsString.resize(0);
	MainStatsString.resize(0);
	PostStatsString.resize(0);
	totalStatsString.append("Total pass: ");
	ShadowStatsString.append("Shadow pass: ");
	MainStatsString.append("Main pass: ");
	PostStatsString.append("Post pass: ");
	
	sprintf_s(a, "%f", 1000.0f * dTDrawTotal);
	totalStatsString.append(a);
	sprintf_s(a, "%f", 1000.0f * m_adT[GTS_ShadowObjects]);
	ShadowStatsString.append(a);
	sprintf_s(a, "%f", 1000.0f * m_adT[GTS_MainObjects]);
	MainStatsString.append(a);
	sprintf_s(a, "%f", 1000.0f * m_adT[GTS_PostPass]);
	PostStatsString.append(a);
	float a6 = 1000.0f * (dTDrawTotal + m_adT[GTS_EndFrame]);

	this->deviceContext->End(m_apQueryTs[GTS_EndFrame][m_iFrameQuery]);
	this->deviceContext->End(m_apQueryTsDisjoint[m_iFrameQuery]);
	++m_iFrameQuery &= 1;


	PrintStats(delta);
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


		D3D11_QUERY_DESC queryDesc = { D3D11_QUERY_TIMESTAMP_DISJOINT, 0 };

		if (FAILED(this->device->CreateQuery(&queryDesc, &m_apQueryTsDisjoint[0])))
			return false;

		if (FAILED(this->device->CreateQuery(&queryDesc, &m_apQueryTsDisjoint[1])))
			return false;

		queryDesc.Query = D3D11_QUERY_TIMESTAMP;

		for (GTS gts = GTS_BeginFrame; gts < GTS_Max; gts = GTS(gts + 1))
		{
			if (FAILED(this->device->CreateQuery(&queryDesc, &m_apQueryTs[gts][0])))
				return false;

			if (FAILED(this->device->CreateQuery(&queryDesc, &m_apQueryTs[gts][1])))
				return false;
		}

	//D3D11_COUNTER_DESC cd;
	//cd.Counter = D3D11_COUNTER_DEVICE_DEPENDENT_0;
	//if(FAILED (this->device->CreateCounter(&cd, &d3dcounter)))
	//	return false;


	//Model *bth = new Model();
	//if (!bth->CreateModel("Models\\bth.righthanded.obj", device))
	//	return false;
	//this->models.push_back(bth);
	//
	//for (size_t i = 0; i < 1; i++)
	//{
	//	Model *bun = new Model();
	//	//if (!bun->CreateModel("Models\\sbun.obj", device))
	//	if (!bun->CreateModel("Models\\bunny.obj", device))
	//		return false;
	//	this->models.push_back(bun);
	//	bun->SetScale(0.1f);
	//	bun->SetPosition(40.0f * i, -80.0f, 0.0f);
	//	//bun->SetPosition(10.0f * i, -112.0f, 150.0f);
	//	this->player = bun;
	//}
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
	//wallSmall->SetScale(5.0f, 5.0f, 1.0f);
	//wallSmall->Rotate(DirectX::SimpleMath::Vector3(10.0f, 0.0f, 0.0f));
	//this->models.push_back(wallSmall);
	//this->player = wallSmall;

// model : Palm tree
	//Model *palm = new Model();
	//if (!palm->CreateModel("Models\\palm.obj", device))
	//	return false;
	//palm->SetPosition(0, -100, 0);
	//palm->SetScale(1.0f, 1.0f, 1.0f);
	//palm->Rotate(DirectX::SimpleMath::Vector3(10.0f, 0.0f, 0.0f));
	//this->models.push_back(palm);

// model : Natlya wamp
	//Model *nat = new Model();
	//if (!nat->CreateModel("Models\\nat.obj", device))
	//	return false;
	//nat->SetPosition(-50, -100, 0);
	//nat->SetScale(10.0f, 10.0f, 10.0f);
	////nat->Rotate(DirectX::SimpleMath::Vector3(10.0f, 0.0f, 0.0f));
	//this->models.push_back(nat);

#pragma region Wall benchmark

	//Model *wall1 = new Model();
	//Model *wall2 = new Model();
	//Model *wall3 = new Model();
	//Model *wall4 = new Model();
	//Model *wall5 = new Model();
	//Model *wallBun = new Model();
	//
	//if (!wall1->CreateModel("Models\\wallSmall1.obj", device)) return false;
	//if (!wall2->CreateModel("Models\\wallSmall1.obj", device)) return false;
	//if (!wall3->CreateModel("Models\\wallSmall1.obj", device)) return false;
	//if (!wall4->CreateModel("Models\\wallSmall1.obj", device)) return false;
	//if (!wall5->CreateModel("Models\\wallSmall1.obj", device)) return false;
	//if (!wallBun->CreateModel("Models\\bunny.obj", device, MaterialLayers::SomeCustomMaterial, Util::NumElementsOf(MaterialLayers::SomeCustomMaterial))) return false;
	//
	//wall1->SetPosition(-300, -100, +10);
	//wall2->SetPosition(-150, -100, +10);
	//wall3->SetPosition(+000, -100, +10);
	//wall4->SetPosition(+150, -100, +10);
	//wall5->SetPosition(+300, -100, +10);
	//wallBun->SetPosition(300, -65, -40);
	//
	//wall1->SetScale(5.0f, 5.0f, 2.0f);
	//wall2->SetScale(5.0f, 5.0f, 4.0f);
	//wall3->SetScale(5.0f, 5.0f, 6.0f);
	//wall4->SetScale(5.0f, 5.0f, 8.0f);
	//wall5->SetScale(5.0f, 5.0f, 10.0f);
	//wallBun->SetScale(0.1f);
	//
	////wallBun->Rotate(SimpleMath::Vector3(0, 90, 0));
	//
	//this->models.push_back(wall1);
	//this->models.push_back(wall2);
	//this->models.push_back(wall3);
	//this->models.push_back(wall4);
	//this->models.push_back(wall5);
	//this->models.push_back(wallBun);
	//
	//this->player = this->models[this->models.size()-1];

#pragma endregion


#pragma region Object benchmark

	int c = 3;
	int i = 0;
	float spacing = 1.0f;
	Model *dragon0 = new Model();
	Model *dragon1 = new Model();
	Model *dragon2 = new Model();
	
	Model *dragon3 = new Model();
	Model *dragon4 = new Model();
	Model *dragon5 = new Model();
	
	Model *dragon6 = new Model();
	Model *dragon7 = new Model();
	Model *dragon8 = new Model();
	Model *dragon9 = new Model();
	Model *dragon10 = new Model();
	Model *dragon11 = new Model();
	
	if (!dragon0->CreateModel("Models\\cube.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	if (!dragon1->CreateModel("Models\\cube.obj", device, MaterialLayers::PigLayers, Util::NumElementsOf(MaterialLayers::PigLayers))) return false;
	if (!dragon2->CreateModel("Models\\cube.obj", device, MaterialLayers::SingleBlueLayer, Util::NumElementsOf(MaterialLayers::SingleBlueLayer))) return false;

	if (!dragon3->CreateModel("Models\\cube.obj", device, MaterialLayers::Material10Layer, Util::NumElementsOf(MaterialLayers::Material10Layer))) return false;
	if (!dragon4->CreateModel("Models\\cube.obj", device, MaterialLayers::Material8Layer, Util::NumElementsOf(MaterialLayers::Material8Layer))) return false;
	if (!dragon5->CreateModel("Models\\cube.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	
	if (!dragon6->CreateModel("Models\\cube.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	if (!dragon7->CreateModel("Models\\cube.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	if (!dragon8->CreateModel("Models\\cube.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	if (!dragon9->CreateModel("Models\\cube.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	if (!dragon10->CreateModel("Models\\cube.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	if (!dragon11->CreateModel("Models\\cube.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	
	dragon0->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	dragon1->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	dragon2->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);

	dragon3->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	dragon4->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	dragon5->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	
	dragon6->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	dragon7->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	dragon8->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	dragon9->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	dragon10->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);
	dragon11->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -80, 10);

	dragon0->SetScale(5.0f);
	dragon1->SetScale(5.0f);
	dragon2->SetScale(5.0f);

	dragon3->SetScale(5.0f);
	dragon4->SetScale(5.0f);
	dragon5->SetScale(5.0f);
	
	dragon6->SetScale(5.0f);
	dragon7->SetScale(5.0f);
	dragon8->SetScale(5.0f);
	dragon9->SetScale(5.0f);
	dragon10->SetScale(5.0f);
	dragon11->SetScale(5.0f);
	
	this->models.push_back(dragon0);
	this->models.push_back(dragon1);
	this->models.push_back(dragon2);

	//this->models.push_back(dragon3);
	//this->models.push_back(dragon4);
	//this->models.push_back(dragon5);
	
	//this->models.push_back(dragon6);
	//this->models.push_back(dragon7);
	//this->models.push_back(dragon8);
	//this->models.push_back(dragon9);
	//this->models.push_back(dragon10);
	//this->models.push_back(dragon11);
	
	this->player = this->models[this->models.size()-1];

#pragma endregion

/*
#pragma region Object benchmark

	int c = 3;
	int i = 0;
	float spacing = 1.0f;
	Model *dragon0 = new Model();
	Model *dragon1 = new Model();
	Model *dragon2 = new Model();
	
	Model *dragon3 = new Model();
	Model *dragon4 = new Model();
	Model *dragon5 = new Model();
	
	Model *dragon6 = new Model();
	Model *dragon7 = new Model();
	Model *dragon8 = new Model();
	Model *dragon9 = new Model();
	Model *dragon10 = new Model();
	Model *dragon11 = new Model();
	
	if (!dragon0->CreateModel("Models\\bench\\dragon0.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	if (!dragon1->CreateModel("Models\\bench\\dragon1.obj", device, MaterialLayers::PigLayers, Util::NumElementsOf(MaterialLayers::PigLayers))) return false;
	if (!dragon2->CreateModel("Models\\bench\\dragon2.obj", device, MaterialLayers::SingleBlueLayer, Util::NumElementsOf(MaterialLayers::SingleBlueLayer))) return false;

	//if (!dragon3->CreateModel("Models\\bench\\dragon3.obj", device, MaterialLayers::Material10Layer, Util::NumElementsOf(MaterialLayers::Material10Layer))) return false;
	//if (!dragon4->CreateModel("Models\\bench\\dragon4.obj", device, MaterialLayers::Material8Layer, Util::NumElementsOf(MaterialLayers::Material8Layer))) return false;
	//if (!dragon5->CreateModel("Models\\bench\\dragon5.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	//
	//if (!dragon6->CreateModel("Models\\bench\\dragon6.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	//if (!dragon7->CreateModel("Models\\bench\\dragon7.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	//if (!dragon8->CreateModel("Models\\bench\\dragon8.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	//if (!dragon9->CreateModel("Models\\bench\\dragon9.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	//if (!dragon10->CreateModel("Models\\bench\\dragon10.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	//if (!dragon11->CreateModel("Models\\bench\\dragon11.obj", device, MaterialLayers::SkinLayers, Util::NumElementsOf(MaterialLayers::SkinLayers))) return false;
	
	dragon0->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	dragon1->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	dragon2->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);

	dragon3->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	dragon4->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	dragon5->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	
	dragon6->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	dragon7->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	dragon8->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	dragon9->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	dragon10->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);
	dragon11->SetPosition(-((float)c / 2.0f) + (float)i++ * spacing, -100, 10);

	dragon0->SetScale(3.0f);
	dragon1->SetScale(3.0f);
	dragon2->SetScale(3.0f);

	dragon3->SetScale(3.0f);
	dragon4->SetScale(3.0f);
	dragon5->SetScale(3.0f);
	
	dragon6->SetScale(3.0f);
	dragon7->SetScale(3.0f);
	dragon8->SetScale(3.0f);
	dragon9->SetScale(3.0f);
	dragon10->SetScale(3.0f);
	dragon11->SetScale(3.0f);
	
	this->models.push_back(dragon0);
	this->models.push_back(dragon1);
	this->models.push_back(dragon2);

	//this->models.push_back(dragon3);
	//this->models.push_back(dragon4);
	//this->models.push_back(dragon5);
	//
	//this->models.push_back(dragon6);
	//this->models.push_back(dragon7);
	//this->models.push_back(dragon8);
	//this->models.push_back(dragon9);
	//this->models.push_back(dragon10);
	//this->models.push_back(dragon11);
	
	this->player = this->models[this->models.size()-1];

#pragma endregion
	*/
	//this->sphereMap.CreateSkyBox(device, dc);

	(this->ground = new Plane())->CreatePlane(device, DirectX::XMFLOAT3(0.0, -130.0f, 0.0f), L"Models\\gray.dds", 1000.0f, 1000.0f, 1.0f);
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
	//size_t totPoint = 25;
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
	//dl.direction = DirectX::XMFLOAT3(0.0f, -0.7f, -0.5f);
	//dl.intensity = 0.2f;
	//this->directionalLight.Push(dl);
	//
	//dl.color = DirectX::XMFLOAT3(0.8f, 0.4f, 0.8f);
	//dl.direction = DirectX::XMFLOAT3(0.9f, -0.9f, 0.9f);
	//this->directionalLight.Push(dl);

#pragma endregion

#pragma region Create shadow and lights
	{
		float spot = 35.0f;
		float range = 1000.0f;
		SimpleMath::Vector3 att(0.001f, 0.001f, 0.0f);
		const int width = 1024;
		const int height = 1024;

		std::vector<pr> s;
		s.push_back (pr(XMFLOAT3(-300.0f, -35.0f, -120.0f) ,XMFLOAT3(45.0f, 0.0f, 0.0f)));
		s.push_back (pr(XMFLOAT3(-150.0f, -35.0f, -120.0f) ,XMFLOAT3(45.0f, 0.0f, 0.0f)));
		s.push_back (pr(XMFLOAT3(+000.0f, -35.0f, -120.0f) ,XMFLOAT3(45.0f, 0.0f, 0.0f)));
		s.push_back (pr(XMFLOAT3(+150.0f, -35.0f, -120.0f) ,XMFLOAT3(45.0f, 0.0f, 0.0f)));
		s.push_back (pr(XMFLOAT3(+300.0f, -35.0f, -120.0f) ,XMFLOAT3(45.0f, 0.0f, 0.0f)));

		//s.push_back (pr(XMFLOAT3(-80.0f, -20.0f, -80.0f) ,XMFLOAT3(+45.0f, +45.0f, +0.0f)));
		//s.push_back (pr(XMFLOAT3(-40.0f, -20.0f, -80.0f) ,XMFLOAT3(+45.0f, +22.0f, +0.0f)));
		//s.push_back (pr(XMFLOAT3(+00.0f, -20.0f, -80.0f) ,XMFLOAT3(+45.0f, +00.0f, +0.0f)));
		//s.push_back (pr(XMFLOAT3(+40.0f, -20.0f, -80.0f) ,XMFLOAT3(+45.0f, -22.0f, +0.0f)));
		//s.push_back (pr(XMFLOAT3(+80.0f, -20.0f, -80.0f) ,XMFLOAT3(+45.0f, -45.0f, +0.0f)));
		//att = SimpleMath::Vector3(0.001f, 0.0f, 0.00002f);
		//range = 200;

		//s.push_back (pr(XMFLOAT3(-400.0f, +300.0f, -330.0f) ,XMFLOAT3(+45.0f, -0.0f, +0.0f)));
		//s.push_back (pr(XMFLOAT3(-200.0f, +300.0f, -330.0f) ,XMFLOAT3(+45.0f, -0.0f, +0.0f)));
		//s.push_back (pr(XMFLOAT3(+000.0f, +300.0f, -330.0f) ,XMFLOAT3(+45.0f, -0.0f, +0.0f)));
		//s.push_back (pr(XMFLOAT3(+200.0f, +300.0f, -330.0f) ,XMFLOAT3(+45.0f, -0.0f, +0.0f)));
		//s.push_back (pr(XMFLOAT3(+400.0f, +300.0f, -330.0f) ,XMFLOAT3(+45.0f, -0.0f, +0.0f)));
		

		//Light casts shadow and sss
		for (size_t i = 0; i < s.size(); i++)
		{
			BasicLightData::ShadowMapLight shadow;
			if(!shadow.shadowMap.Create(this->device, this->deviceContext, width, height))
				break;

			shadow.color.x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			shadow.color.y = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			shadow.color.z = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			shadow.color = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);

			shadow.attenuation		= att;
			shadow.spot				= spot;
			shadow.range			= range;
		
			shadow.camera.SetProjectionMatrix(DirectX::XMConvertToRadians(55.0f), ((float)this->desc.width/(float)this->desc.height), 0.1f, range);
			shadow.camera.SetPosition(s[i].p);
			shadow.camera.SetRotation(s[i].r);
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
void Renderer::RenderGeometry()
{
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
	}

	Pipeline::PipelineManager::Instance().ApplyGeometryPass(this->mainCam->GetViewMatrix(), this->mainCam->GetProjectionMatrix(), shadowBuff, k);
	{
		for (size_t i = 0; i < this->models.size(); i++)
		{
			this->models[i]->DrawModel(this->deviceContext);
		}
	}
}
void Renderer::RenderPostPass(float dt)
{
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
}

void Renderer::PrintStats(float dt)
{
	
	//if (time > timeMax)
	{
		TextRender::Write(Util::StringToWstring(totalStatsString, std::wstring()).c_str(), 10, 210);
		TextRender::Write(Util::StringToWstring(ShadowStatsString, std::wstring()).c_str(), 10, 230);
		TextRender::Write(Util::StringToWstring(MainStatsString, std::wstring()).c_str(), 10, 250);
		TextRender::Write(Util::StringToWstring(PostStatsString, std::wstring()).c_str(), 10, 270);
	}
}


