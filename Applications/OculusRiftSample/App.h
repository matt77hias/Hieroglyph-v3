//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "RenderApplication.h"
#include "Actor.h"
#include "GeometryActor.h"
#include "TextActor.h"
#include "PointLight.h"
#include "FullscreenTexturedActor.h"

#include "GlyphRift\RiftManager.h"
#include "GlyphRift\RiftHMD.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
class App : public RenderApplication
{
public:
	App();
	
	virtual bool ConfigureRenderingEngineComponents( UINT width, UINT height, D3D_FEATURE_LEVEL desiredLevel, D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE );

	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual bool ConfigureRenderingSetup();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual void HandleWindowResize( HWND handle, UINT width, UINT height );
	virtual bool HandleEvent( EventPtr pEvent );
	virtual std::wstring GetName( );

protected:
	PointLight*					m_pLight;
	GeometryActor*				m_pIndexedActor;
	GeometryActor*				m_pGeometryActor;
	TextActor*					m_pTextActor;
	Actor*						m_pMeshActor;
	ResourcePtr					m_MirrorTexture;
	FullscreenTexturedActor*	m_pFullscreenTexturedActor;

public:
	RiftManagerPtr m_pRiftMgr;
	RiftHMDPtr m_pRiftHmd;
};
//--------------------------------------------------------------------------------