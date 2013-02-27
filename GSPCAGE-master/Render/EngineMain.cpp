/************************************************************************
*  Program Name: Engine main.cpp                                               
*  Name: GSP420 Rendering Core - Mike Murrell, Anthony Garza, Jesse Goldan, Christopher Carlos                                                                                         *
*  Date: January 28th - February 1st
*  Description: Mainly for the rendering of the engine
*  Update: February 8, 2013
//=============================================================================
// Modified from Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================   
************************************************************************/


#include "EngineMain.h"

cResourceManager* resMan = new cResourceManager();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	EngineMain app(hInstance, L"GSP420 Engine", D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING);
	g_d3dApp = &app;

	DirectInput di(DISCL_NONEXCLUSIVE|DISCL_FOREGROUND, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND);
	gDInput = &di;

    return g_d3dApp->run();
}

EngineMain::EngineMain(HINSTANCE hInstance, std::wstring winCaption, D3DDEVTYPE devType, DWORD requestedVP)
: D3DApp(hInstance, winCaption, devType, requestedVP)
{

	if(!checkDeviceCaps())
	{
		MessageBox(0, L"checkDeviceCaps() Failed", 0, 0);
		PostQuitMessage(0);
	}

	
	Shaders::InitAll();
	InitAllVertexDeclarations();
	

	m_GfxStats = new GfxStats();

	m_DirLight.Direction	= D3DXVECTOR3(0.0f, 1.0f, 2.0f);
	D3DXVec3Normalize(&m_DirLight.Direction, &m_DirLight.Direction);
	m_DirLight.Ambient		= D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.Diffuse		= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight.Specular		= D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.0f);

	camera.SetPosition(D3DXVECTOR3(0.0f, 0.0f, -10.0f));
	camera.SetTarget(D3DXVECTOR3(0.0f, 3.0f, 0.0f));
	camera.SetRotation(D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	

	dwarf =  new Model(L"dwarf.x");
	skull =	 new Model(L"skullocc.x");
	tiny  =  new Model(L"tiny.x");

	m_GfxStats->addVertices(skull->m_Model->GetNumVertices());
	m_GfxStats->addTriangles(skull->m_Model->GetNumFaces());
	m_GfxStats->addVertices(dwarf->m_Model->GetNumVertices());
	m_GfxStats->addTriangles(dwarf->m_Model->GetNumFaces());
	m_GfxStats->addVertices(tiny->m_Model->GetNumVertices());
	m_GfxStats->addTriangles(tiny->m_Model->GetNumFaces());

	skull->InitModel(D3DXVECTOR3(-4.0f, 3.0f, 100.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), 0.0f);
	skull->Scale(0.5f);
	dwarf->InitModel(D3DXVECTOR3(0.0f, 3.0f, -10.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), 0.0f);
	dwarf->Scale(1.0f);
	tiny->InitModel(D3DXVECTOR3(0.0f, 3.0f, -10.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), 0.0f);
	tiny->Scale(0.01f);

	onResetDevice();
}

EngineMain::~EngineMain()
{
	delete m_GfxStats;
	delete(skull);
	delete(dwarf);
	delete(tiny);
	DestroyAllVertexDeclarations();
	Shaders::DestroyAll();
}

bool EngineMain::checkDeviceCaps()
{
	D3DCAPS9 caps;
	HR(g_d3dDevice->GetDeviceCaps(&caps));

	// Check for vertex shader version 3.0 support.
	if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) )
		return false;

	// Check for pixel shader version 3.0 support.
	if( caps.PixelShaderVersion < D3DPS_VERSION(2, 0) )
		return false;

	return true;
}

void EngineMain::onLostDevice()
{
	m_GfxStats->onLostDevice();
	HR(Shaders::BasicFX->m_FX->OnLostDevice());
	HR(Shaders::VBlendFX->m_FX->OnLostDevice());
}

void EngineMain::onResetDevice()
{
	m_GfxStats->onResetDevice();
	HR(Shaders::BasicFX->m_FX->OnResetDevice());
	HR(Shaders::VBlendFX->m_FX->OnResetDevice());

	// The aspect ratio depends on the backbuffer dimensions, which can 
	// possibly change after a reset.  So rebuild the projection matrix.
	buildProjMtx();
}

void EngineMain::updateScene(float dt)
{
	m_GfxStats->update(dt);

	// Get snapshot of input devices.
	/*gDInput->poll();

	// Check input.
	if( gDInput->keyDown(DIK_W) )
		dwarf->Move(0, 5.0f * dt);
	if( gDInput->keyDown(DIK_S) )	 
		dwarf->Move(0, -5.0f * dt);
	if( gDInput->keyDown(DIK_A) )	 
		dwarf->Move(-5.0f * dt, 0);
	if( gDInput->keyDown(DIK_D) )	
	{
		dwarf->Move(5.0f * dt, 0);
		//camera.Move(D3DXVECTOR3(5.0f, 0.0f, 0.0f) * dt);
	}
	if( gDInput->keyDown(DIK_L) )
		dwarf->Rotate(5.0f * dt);
	if( gDInput->keyDown(DIK_K) )
		dwarf->Rotate(-5.0f * dt);*/

	// The camera position/orientation relative to world space can 
	// change every frame based on input, so we need to rebuild the
	// view matrix every frame with the latest changes.
	buildViewMtx();
	skull->Rotate(sin(dt));
	tiny->Rotate(sin(dt));
}


void EngineMain::drawScene()
{
	// Clear the backbuffer and depth buffer.
	HR(g_d3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0));
	
	HR(g_d3dDevice->BeginScene());

	D3DXMATRIX wVPM = camera.GetView()*camera.GetProjection();

	// Setup the rendering FX
	Shaders::BasicFX->SetDirectionalLight(&m_DirLight);
	Shaders::BasicFX->m_FX->SetTechnique(Shaders::BasicFX->m_hTech);
	// Begin passes.
	UINT numPasses = 0;
	Shaders::BasicFX->m_FX->Begin(&numPasses, 0);
	Shaders::BasicFX->m_FX->BeginPass(0);
	skull->DrawModel(wVPM);
	dwarf->DrawModel(wVPM);
	tiny->DrawModel(wVPM);

	Shaders::BasicFX->m_FX->EndPass();
	Shaders::BasicFX->m_FX->End();
	// End //

	// Animation Passes //
	Shaders::VBlendFX->SetDirectionalLight(&m_DirLight);
	//HR(Shaders::VBlendFX->m_FX->SetValue(Shaders::VBlendFX->m_hLight, &m_DirLight, sizeof(DirectionalLight))); 
	Shaders::VBlendFX->m_FX->SetTechnique(Shaders::VBlendFX->m_hTech);
	numPasses = 0;
	Shaders::VBlendFX->m_FX->Begin(&numPasses, 0);
	Shaders::VBlendFX->m_FX->BeginPass(0);


	//tiny->draw(wVPM);

	Shaders::VBlendFX->m_FX->EndPass();
	Shaders::VBlendFX->m_FX->End();
	// End //

	m_GfxStats->display();

	HR(g_d3dDevice->EndScene());

	// Present the backbuffer.
	HR(g_d3dDevice->Present(0, 0, 0, 0));
}

void EngineMain::buildViewMtx()
{

	camera.Render();
	Shaders::BasicFX->SetEyePosition(camera.GetPosition());
}

void EngineMain::buildProjMtx()
{
	float w = (float)m_d3dPP.BackBufferWidth;
	float h = (float)m_d3dPP.BackBufferHeight;
	camera.SetAspectRatio(w/h);
	D3DXMatrixPerspectiveFovLH(&camera.GetProjection(), camera.GetFOV(), camera.GetAspectRatio(), camera.GetNearValue(), camera.GetFarValue());
}

