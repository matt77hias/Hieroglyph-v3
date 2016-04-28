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

#include "VertexElementDX11.h"
#include "GeometryDX11.h"
#include "RenderEffectDX11.h"

#include "SpriteRendererDX11.h"
#include "SpriteFontLoaderDX11.h"

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

	bool					m_bGeometryMode;
	static const bool		TRI_MODE = true;
	static const bool		QUAD_MODE = false;

	GeometryPtr				m_pQuadGeometry;
	RenderEffectDX11*		m_pQuadEffect;

	GeometryPtr				m_pTriangleGeometry;
	RenderEffectDX11*		m_pTriangleEffect;

	SpriteFontPtr			m_pFont;
	SpriteRendererDX11*		m_pSpriteRenderer;

	bool					m_bEdgeOrInsideMode;
	static const bool		EDGE_MODE = true;
	static const bool		INSIDE_MODE = false;

	float					m_fEdgeWeights[4];
	unsigned int			m_iEdgeIndex;

	float					m_fInsideWeights[2];
	unsigned int			m_iInsideIndex;

	enum PartitioningMode
	{
		Power2,
		Integer,
		FractionalOdd,
		FractionalEven
	};

	PartitioningMode		m_pmPartitioningMode;
	std::map<PartitioningMode, int> m_QuadHullShaders;
	std::map<PartitioningMode, int> m_TriangleHullShaders;

	virtual void CreateQuadResources();
	virtual void CreateTriangleResources();

	virtual void SetEdgeWeight(unsigned int index, float weight);
	virtual void SetInsideWeight(unsigned int index, float weight);

	virtual void SetPartitioningMode( const PartitioningMode& mode );
};
