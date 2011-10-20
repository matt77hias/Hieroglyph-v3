
#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"



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

	// Input Texture IDs
	ResourcePtr				m_Texture;

	// Output Texture IDs
	ResourcePtr				m_Output;

	// Geometry for full screen pass
	GeometryPtr				m_pFullScreen;

	// RenderEffects for running the compute shader and rendering
	// the resulting texture to the backbuffer.
	RenderEffectDX11*		m_pFilterEffect;
	RenderEffectDX11*		m_pTextureEffect;

	bool					m_bSaveScreenshot;
};
