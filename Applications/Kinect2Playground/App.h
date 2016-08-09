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
#include "PointLight.h"
#include "GlyphKinect2\Kinect2Manager.h"
#include "GlyphKinect2\Kinect2SkeletonActor.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
class App : public RenderApplication
{

public:
	App();

	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool HandleEvent( EventPtr pEvent );
	virtual std::wstring GetName( );

protected:
	std::unique_ptr<Kinect2Manager>	m_pKinectManager;
	PointLight*						m_pLight;
	Kinect2SkeletonActor*			m_pSkeletonActor;
};
//--------------------------------------------------------------------------------