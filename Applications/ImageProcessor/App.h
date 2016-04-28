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

#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "VectorParameterDX11.h"
#include "SamplerParameterDX11.h"
#include "Vector4f.h"

using namespace Glyph3;

class App : public RenderApplication
{

public:
	App();
	
public:

	virtual void MessageLoop();
	virtual LRESULT WindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool HandleEvent( EventPtr pEvent );

	virtual bool HandleResize( HWND hwnd, WPARAM wparam, LPARAM lparam );
	virtual bool HandleMouseMove( HWND hwnd, WPARAM wparam, LPARAM lparam );
	virtual bool HandleMouseWheel( HWND hwnd, WPARAM wparam, LPARAM lparam );

	void SelectNextImage();
	void SelectNextAlgorithm();
	void SelectNextSampler();

	virtual std::wstring GetName( );

protected:
	bool					m_bAppInitialized;
	bool					m_bAppShuttingDown;

	// Texture Resources
	ResourcePtr				m_Texture[5];
	ResourcePtr				m_Intermediate;
	ResourcePtr				m_Output;

	int						m_Samplers[2];

	Actor*					m_pActor;

	RenderEffectDX11*		m_pBruteForceGaussian;
	RenderEffectDX11*		m_pSeparableGaussianX;
	RenderEffectDX11*		m_pSeparableGaussianY;
	RenderEffectDX11*		m_pCachedGaussianX;
	RenderEffectDX11*		m_pCachedGaussianY;

	RenderEffectDX11*		m_pBruteForceBilateral;
	RenderEffectDX11*		m_pSeparableBilateralX;
	RenderEffectDX11*		m_pSeparableBilateralY;

	ShaderResourceParameterDX11* m_pColorMapParameter;
	ShaderResourceParameterDX11* m_pInputParameter;
	UnorderedAccessParameterDX11* m_pOutputParameter;

	VectorParameterDX11*	m_pWindowSizeParameter;
	VectorParameterDX11*	m_pImageSizeParameter;
	VectorParameterDX11*	m_pViewingParamsParameter;

	Vector4f				WindowSize;
	Vector4f				ImageSize;
	Vector4f				ViewingParams;

	int						m_iAlgorithm;
	int						m_iImage;
	int						m_iSampler;

	struct UserInteractionData
	{
		bool LMouseDown;
		bool RMouseDown;
        int LastMouseX;
        int LastMouseY;
        int MouseDeltaX;
        int MouseDeltaY;
	};

	UserInteractionData m_UIData;

};
