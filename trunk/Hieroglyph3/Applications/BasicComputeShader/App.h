
#include "Application.h"
#include "IEventListener.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"



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
	
	int						m_iSwapChain;
	int						m_iRenderTarget;
	int						m_iDepthTarget;

	// Input Texture IDs
	int						m_iTexture;
	int						m_iTextureSRV;

	// Output Texture IDs
	int						m_iOutput;
	int						m_iOutputUAV;
	int						m_iOutputSRV;

	// Geometry for full screen pass
	GeometryDX11*			m_pFullScreen;

	// RenderEffects for running the compute shader and rendering
	// the resulting texture to the backbuffer.
	RenderEffectDX11*		m_pFilterEffect;
	RenderEffectDX11*		m_pTextureEffect;

	bool					m_bSaveScreenshot;
};
