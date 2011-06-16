
#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"
#include "SpriteRendererDX11.h"
#include "SpriteFontDX11.h"

#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"
#include "TArray.h"

#include "ViewGBuffer.h"
#include "ViewLights.h"
#include "AppSettings.h"

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

    void SetupViews();
    void DrawHUD();

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow;

    TArray<ResourcePtr>		m_GBuffer[GBufferOptMode::NumSettings][AAMode::NumSettings];
	ResourcePtr				m_DepthTarget[AAMode::NumSettings];
    ResourcePtr             m_ReadOnlyDepthTarget[AAMode::NumSettings];
    ResourcePtr             m_FinalTarget[AAMode::NumSettings];
    int                     m_iViewport[AAMode::NumSettings];
    ResourcePtr             m_ResolveTarget;
	ResourcePtr				m_BackBuffer;

    MaterialDX11*           m_pMaterial;
    RenderEffectDX11*       m_pGBufferEffect[GBufferOptMode::NumSettings];
    int					    m_iGBufferDSState;
    int                     m_iGBufferRSState;

    ResourcePtr             m_DiffuseTexture;
    ResourcePtr             m_NormalMap;

	ViewGBuffer*			m_pGBufferView;
    ViewLights*             m_pLightsView;
	SpriteRendererDX11		m_SpriteRenderer;
	SpriteFontDX11			m_Font;

	Node3D*					m_pNode;
	Entity3D*				m_pEntity;

	Camera*					m_pCamera;

    int                     m_vpWidth;
    int                     m_vpHeight;

	bool					m_bSaveScreenshot;
};