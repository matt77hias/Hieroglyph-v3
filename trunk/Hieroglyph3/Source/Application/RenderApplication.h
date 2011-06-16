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
	
public:
	bool ConfigureRenderingEngineComponents( int width, int height, D3D_FEATURE_LEVEL desiredLevel );

protected:

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow;

	int						m_iWidth;
	int						m_iHeight;

	ResourcePtr				m_RenderTarget;
	ResourcePtr				m_DepthTarget;

	ViewPerspective*		m_pRenderView;
	ViewTextOverlay*		m_pTextOverlayView;

	Camera*					m_pCamera;

	bool					m_bSaveScreenshot;
};
