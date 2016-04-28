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

#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "VectorParameterDX11.h"
#include "MatrixParameterDX11.h"
#include "SamplerParameterDX11.h"

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

	RendererDX11*				m_pRenderer11;
	Win32RenderWindow*			m_pWindow;

	bool						m_bViewPointInAutoMode;
	bool						m_bSolidRender;
	bool						m_bSimpleComplexity;
	
	int							m_iSwapChain;
	ResourcePtr					m_RenderTarget;
	ResourcePtr					m_DepthTarget;

	SpriteFontPtr				m_pFont;
	SpriteRendererDX11*			m_pSpriteRenderer;

	GeometryPtr					m_pTerrainGeometry;
	RenderEffectDX11*			m_pTerrainEffect;
	ResourcePtr					m_pHeightMapTexture;
	RenderEffectDX11*			m_pComputeShaderEffect;
	ResourcePtr					m_pLodLookupTexture;

	int							m_rsWireframe;
	int							m_rsSolid;

	static const int			TERRAIN_X_LEN = 32;
	static const int			TERRAIN_Z_LEN = 32;

	enum ShadingMode
	{
		SolidColour,
		SimpleShading,
		LodDebugView
	};

	ShadingMode					m_smCurrentShading;
	std::map<ShadingMode, int>	m_TerrainDomainShaders;

	int							m_iComplexHullShader;
	int							m_iSimpleHullShader;

	virtual void CreateTerrainGeometry();
	virtual void CreateTerrainShaders();
	virtual void CreateTerrainTextures();
	virtual void UpdateViewState();
	virtual void CreateComputeShaderResources();
	virtual void RunComputeShader();

	UnorderedAccessParameterDX11*	m_pbufferResults;
	ShaderResourceParameterDX11*	m_ptexLODLookup;
	ShaderResourceParameterDX11*	m_ptexHeightMap;
	MatrixParameterDX11*			m_pmWorld;
	MatrixParameterDX11*			m_pmViewProj;
	MatrixParameterDX11*			m_pmInvTposeWorld;
	VectorParameterDX11*			m_pcameraPosition;
	VectorParameterDX11*			m_pheightMapDimensions;
	VectorParameterDX11*			m_pminMaxDistance;
	VectorParameterDX11*			m_pminMaxLOD;
	SamplerParameterDX11*			m_psmpHeightMap;
};
