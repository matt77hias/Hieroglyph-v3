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
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
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
	PointLight*					m_pLight;
	GeometryActor*				m_pIndexedActor;
	GeometryActor*				m_pGeometryActor;
	TextActor*					m_pTextActor;
	Actor*						m_pMeshActor;
	GeometryActor*				m_pTextureActor;
};
//--------------------------------------------------------------------------------