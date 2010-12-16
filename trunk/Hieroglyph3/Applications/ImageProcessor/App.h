
#include "Application.h"
#include "IEventListener.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"

using namespace Glyph3;

class App : public Application, public IEventListener
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

	ResourcePtr				m_RenderTarget;
	ResourcePtr				m_DepthTarget;

	// Texture Resources
	ResourcePtr				m_Texture;
	ResourcePtr				m_Intermediate;
	ResourcePtr				m_Output;


	ViewPerspective*		m_pRenderView;
	Node3D*					m_pNode;
	Entity3D*				m_pEntity;

	RenderEffectDX11*		m_pBruteForceGaussian;
	RenderEffectDX11*		m_pSeparableGaussianX;
	RenderEffectDX11*		m_pSeparableGaussianY;
	RenderEffectDX11*		m_pCachedGaussianX;
	RenderEffectDX11*		m_pCachedGaussianY;
	
	Camera*					m_pCamera;

	bool					m_bSaveScreenshot;
};
