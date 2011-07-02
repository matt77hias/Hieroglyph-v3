#include "PCH.h"

#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"
#include "ViewTextOverlay.h"
#include "Camera.h"
#include "Scene.h"

using namespace Glyph3;

class RenderApplication : public Application
{

public:
	RenderApplication();
	virtual ~RenderApplication();
	
public:
	bool ConfigureRenderingEngineComponents( int width, int height, D3D_FEATURE_LEVEL desiredLevel, D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE );

	virtual bool HandleEvent( IEvent* pEvent );


protected:

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow;

	int						m_iWidth;
	int						m_iHeight;

	ResourcePtr				m_RenderTarget;
	ResourcePtr				m_DepthTarget;

	ViewPerspective*		m_pRenderView;
	ViewTextOverlay*		m_pTextOverlayView;

public:
	Camera*					m_pCamera;

	bool					m_bSaveScreenshot;
};
