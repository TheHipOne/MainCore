/************************************************************************
*  Program Name: Engine main.h                                              
*  Name: GSP420 Rendering Core - Mike Murrell, Anthony Garza, Jesse Goldan, Christopher Carlos                                                                                         *
*  Date: January 28th - February 1st
*  Description: Mainly for the rendering of the engine
*  Update: February 8, 2013
//=============================================================================
// Modified from Frank Luna (C) 2005 All Rights Reserved.
//=============================================================================   
************************************************************************/
#ifndef	ENGINEMAIN_H_
#define ENGINEMAIN_H_

#include "..\Render\d3dApp.h"
#include "..\Render\DirectInput.h"
#include <crtdbg.h>
#include "..\Render\GfxStats.h"
#include <list>
#include "..\Render\Vertex.h"
#include "..\Render\Lighting.h"
#include "..\Render\Camera.h"
#include "..\Render\Shaders.h"
#include "..\Render\Model.h"
#include "..\Render\AnimatedModel.h"

class EngineMain : public D3DApp
{
public:
	EngineMain(HINSTANCE hInstance, std::wstring winCaption, D3DDEVTYPE devType, DWORD requestedVP);
	~EngineMain();

	bool checkDeviceCaps();
	void onLostDevice();
	void onResetDevice();
	void updateScene(float dt);
	void drawScene();

	// Helper methods
	void buildFX();
	void buildViewMtx();
	void buildProjMtx();
	void drawMeshes();

private:
	GfxStats* m_GfxStats;

	// // // Models // // //
	Model*			skull;
	Model*			dwarf;
	Model*	tiny;
	// // // // // // // // 

	DirectionalLight m_DirLight;

	Camera camera;

	D3DXMATRIX m_World;

};

#endif