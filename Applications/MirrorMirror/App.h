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

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"
#include "Actor.h"

#include "SpriteRendererDX11.h"
#include "SpriteFontDX11.h"

#include "ReflectiveSphereEntity.h"
#include "DiffuseSphereEntity.h"
#include "VectorParameterDX11.h"

using namespace Glyph3;

class App : public RenderApplication
{

public:
	App();
	
public:
	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool HandleEvent( EventPtr pEvent );
	virtual std::wstring GetName( );

protected:

	Actor*									m_pActor;
	std::vector<ReflectiveSphereEntity*>	m_vReflectors;
	Actor*									m_pDiffuseActor;


	Vector4f								m_LightParams;
	Vector4f								m_LightPosition;

	VectorParameterDX11*					m_pLightColor;
	VectorParameterDX11*					m_pLightPosition;
	VectorParameterDX11*					m_pTimeFactors;
	VectorParameterDX11*					m_pViewPosition;
};
