
#include "Application.h"
#include "IEventListener.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "VertexElementDX11.h"
#include "GeometryDX11.h"
#include "RenderEffectDX11.h"

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

	bool					m_bGeometryMode;
	const bool				TRI_MODE = true;
	const bool				QUAD_MODE = false;

	GeometryDX11*			m_pQuadGeometry;
	RenderEffectDX11*		m_pQuadEffect;

	GeometryDX11*			m_pTriangleGeometry;
	RenderEffectDX11*		m_pTriangleEffect;

	bool					m_bSaveScreenshot;

	virtual void CreateQuadResources();
	virtual void CreateTriangleResources();
};
