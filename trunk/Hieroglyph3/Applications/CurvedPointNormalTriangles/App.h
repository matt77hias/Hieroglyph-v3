
#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "Entity3D.h"
#include "Node3D.h"
#include "ViewPerspective.h"

#include "Camera.h"
#include "Scene.h"

#include "GeometryLoaderDX11.h"
#include "GeometryDX11.h"

#include "SpriteRendererDX11.h"
#include "SpriteFontDX11.h"

#include "VectorParameterDX11.h"

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

	virtual bool HandleEvent( IEvent* pEvent );
	virtual std::wstring GetName( );

protected:

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow;
	
	int						m_iSwapChain;
	ResourcePtr				m_RenderTarget;
	ResourcePtr				m_DepthTarget;

	SpriteFontDX11*			m_pFont;
	SpriteRendererDX11*		m_pSpriteRenderer;

	bool					m_bSaveScreenshot;
	bool					m_bSolidRender;
	bool					m_bDefaultComplexity;
	
	GeometryPtr				m_pGeometry;
	RenderEffectDX11*		m_pEffect;

	int						m_iDefaultHullShader;
	int						m_iSilhouetteHullShader;
	int						m_rsWireframe;
	int						m_rsSolid;

	float					m_fTessFactor;

	Camera*		m_pCamera;

	VectorParameterDX11* m_pEdgeFactors;

	virtual void CreateShaders();
	virtual void UpdateViewState();
};
