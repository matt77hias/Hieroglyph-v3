
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
	ResourcePtr				m_RenderTarget;
	ResourcePtr				m_DepthTarget;

	bool					m_bSaveScreenshot;

	RenderEffectDX11*		m_pTessellationEffect;
	GeometryDX11*			m_pGeometry;

	Matrix4f				m_WorldMatrix;
	Matrix4f				m_ViewMatrix;
	Matrix4f				m_ProjMatrix;
	Matrix4f				m_ViewProjMatrix;

	Vector4f				m_TessParams;
};
