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
#include "GlyphletActor.h"
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

	virtual bool HandleEvent( EventPtr pEvent );
	virtual std::wstring GetName( );

protected:
	unsigned int					m_uiInputTarget;

	TextActor*						m_pHeadline1;
	TextActor*						m_pHeadline2;
	TextActor*						m_pBodyText1;
	TextActor*						m_pBodyText2;

	GlyphletActor*					m_pActor1;
	GlyphletActor*					m_pActor2;
};
