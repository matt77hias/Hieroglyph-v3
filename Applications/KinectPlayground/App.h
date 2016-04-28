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

#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"

#include "VectorParameterDX11.h"
#include "GlyphKinect\ViewKinect.h"
#include "GlyphKinect\KinectSkeletonActor.h"

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

	ViewKinect*				m_pKinectView;

	Node3D*					m_pNode;
	
	VectorParameterDX11*	m_pLightColor;
	VectorParameterDX11*	m_pLightPosition;

	KinectSkeletonActor*	m_pSkeletonActor;
	Actor*					m_pDepthMapViewer;
	Actor*					m_pColorActor;
	Actor*					m_pDepthActor;
};
