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

#include "SpriteRendererDX11.h"
#include "SpriteFontDX11.h"

#include "VectorParameterDX11.h"
#include "ParticleSystemActor.h"

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

	ParticleSystemActor*	m_pActor;

	VectorParameterDX11*	m_pTimeFactors;
	VectorParameterDX11*	m_pEmitterLocation;
	VectorParameterDX11*	m_pConsumerLocation;
};
