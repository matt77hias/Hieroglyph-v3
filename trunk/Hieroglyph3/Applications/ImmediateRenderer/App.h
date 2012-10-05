//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------
#include "RenderApplication.h"
#include "Actor.h"
#include "ImmediateGeometryDX11.h"
#include "IndexedImmediateGeometryDX11.h"
#include "GeometryActor.h"
#include "TextActor.h"

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

	virtual bool HandleEvent( IEvent* pEvent );
	virtual std::wstring GetName( );

protected:

	Actor*						m_pActor;
	ImmediateGeometryPtr		m_pGeometry;

	Actor*						m_pIndexedActor;
	IndexedImmediateGeometryPtr m_pIndexedGeometry;

	TextActor*					m_pTextActor;

	GeometryActor*				m_pGeometryActor;
};
