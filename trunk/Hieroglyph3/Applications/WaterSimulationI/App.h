
#include "Application.h"
#include "IEventListener.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"

#include "SpriteRendererDX11.h"
#include "SpriteFontDX11.h"

#include "VectorParameterDX11.h"

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
	Entity3D*				m_pEntity;
	
	Camera*					m_pCamera;

	SpriteFontDX11*			m_pFont;
	SpriteRendererDX11*		m_pSpriteRenderer;

	bool					m_bSaveScreenshot;

	// Add parameter references here for setting parameters.
	VectorParameterDX11*	m_pTimeFactors;
	VectorParameterDX11*	m_pDispatchSize;
	VectorParameterDX11*	m_pFinalColor;
};
