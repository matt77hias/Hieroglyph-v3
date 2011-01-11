
#include "Application.h"
#include "IEventListener.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"
#include "Actor.h"

#include "SpriteRendererDX11.h"
#include "SpriteFontDX11.h"

#include "ReflectiveSphereEntity.h"
#include "DiffuseSphereEntity.h"

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

	ViewPerspective*		m_pRenderView;
	Node3D*					m_pNode;
	DiffuseSphereEntity*	m_pEntity;
	ReflectiveSphereEntity*	m_pReflector[16];
	
	Camera*					m_pCamera;
	Actor*					m_pDiffuseActor;
	Actor*					m_pReflectiveActor;

	Vector4f				m_LightParams;
	Vector4f				m_LightPosition;

	SpriteFontDX11*			m_pFont;
	SpriteRendererDX11*		m_pSpriteRenderer;

	bool					m_bSaveScreenshot;
};
