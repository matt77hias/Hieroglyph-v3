
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
#include "ViewFinalPass.h"
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

    void SetupLights();
    void DrawHUD();

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow;

    ResourcePtr     		m_GBufferTarget;
    ResourcePtr             m_LightTarget;
	ResourcePtr				m_DepthTarget;
    ResourcePtr             m_ReadOnlyDepthTarget;
    ResourcePtr             m_FinalTarget;
    int                     m_iViewport;
    ResourcePtr             m_ResolveTarget;
	ResourcePtr				m_BackBuffer;

    MaterialPtr				m_pMaterial;
    RenderEffectDX11*       m_pGBufferEffect;
    RenderEffectDX11*       m_pFinalPassEffect;
    int					    m_iGBufferDSState;
    int                     m_iFinalPassDSState;
    int                     m_iRSState;

    ResourcePtr             m_DiffuseTexture;
    ResourcePtr             m_NormalMap;

	ViewGBuffer*			m_pGBufferView;
    ViewLights*             m_pLightsView;
    ViewFinalPass*          m_pFinalPassView;
	SpriteRendererDX11		m_SpriteRenderer;
	SpriteFontDX11			m_Font;

	Node3D*					m_pNode;
	Entity3D*				m_pEntity;

	Camera*					m_pCamera;

    int                     m_vpWidth;
    int                     m_vpHeight;

	bool					m_bSaveScreenshot;
};