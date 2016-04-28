//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"

#include "VectorParameterDX11.h"

using namespace Glyph3;

class App : public Application
{

public:
	App();
	
public:
	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual void TakeScreenShot();

	virtual bool HandleEvent( EventPtr pEvent );
	virtual std::wstring GetName( );

protected:
#define NUM_WINDOWS 20

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow[NUM_WINDOWS];

	ResourcePtr				m_RenderTarget[NUM_WINDOWS];
	ResourcePtr				m_DepthTarget;
	ResourcePtr				m_OffscreenTexture;

	ViewPerspective*		m_pRenderView;
	Actor*					m_pActor;
	
	GeometryPtr				m_pGeometry;
	MaterialPtr				m_pMaterial;
	
	Vector4f				m_LightParams;
	Vector4f				m_LightPosition;

	Camera*					m_pCamera;
	Vector2f				m_DesktopRes;

	VectorParameterDX11*	m_pLightColor;
	VectorParameterDX11*	m_pLightPosition;
};
