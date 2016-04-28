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

#include "Entity3D.h"
#include "Actor.h"
#include "ViewPerspective.h"

#include "Camera.h"
#include "Scene.h"

#include "GeometryLoaderDX11.h"
#include "GeometryDX11.h"

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

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow;
	
	int						m_iSwapChain;
	ResourcePtr				m_RenderTarget;
	ResourcePtr				m_DepthTarget;

	ViewPerspective*		m_pRenderView;
	Actor*					m_pActor;
	Entity3D*				m_pEntity[10];
	
	GeometryPtr				m_pGeometry;
	MaterialPtr				m_pMaterial;
	
	Vector4f				m_TessParams;

	Camera*					m_pCamera;
};
