
#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"

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
#define NUM_WINDOWS 20

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow[NUM_WINDOWS];

	ResourcePtr				m_RenderTarget[NUM_WINDOWS];
	ResourcePtr				m_DepthTarget;
	ResourcePtr				m_OffscreenTexture;

	ViewPerspective*		m_pRenderView;
	Node3D*					m_pNode;
	Entity3D*				m_pEntity;
	
	GeometryDX11*			m_pGeometry;
	MaterialDX11*			m_pMaterial;
	
	Vector4f				m_LightParams;
	Vector4f				m_LightPosition;

	Camera*					m_pCamera;
	Vector2f				m_DesktopRes;

	bool					m_bSaveScreenshot;

	VectorParameterDX11*	m_pLightColor;
	VectorParameterDX11*	m_pLightPosition;
};
