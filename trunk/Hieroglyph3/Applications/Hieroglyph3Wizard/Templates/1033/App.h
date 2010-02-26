
#include "Application.h"
#include "IEventListener.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "Entity3D.h"
#include "Node3D.h"
#include "ViewPerspective.h"

#include "Camera.h"
#include "Scene.h"

#include "GeometryLoaderDX11.h"
#include "GeometryDX11.h"

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

	bool					m_bSaveScreenshot;

	ViewPerspective*		m_pRenderView;
	Entity3D*				m_pEntity[10];
	
	GeometryDX11*			m_pGeometry;
	MaterialDX11*			m_pMaterial;
	
	Vector4f				m_TessParams;

	Camera*		m_pCamera;
	Scene*		m_pScene;
};
