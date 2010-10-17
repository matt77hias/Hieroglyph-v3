
#include "Application.h"
#include "IEventListener.h"

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
    
    void SetupViews();
    void DrawHUD();

    enum DisplayMode 
    {
        Final = 0, 
        GBuffer = 1, 
        Normals = 2,        
        DiffuseAlbedo = 3,
        SpecularAlbedo = 4,
        Position = 5,

        NumDisplayModes
    };

    enum LightMode
    {       
        Lights3x3x3 = 0,
        Lights5x5x5,
        Lights7x7x7,

        NumLightModes
    };    

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow;

	TArray<ResourcePtr>		m_GBuffer;    
    TArray<ResourcePtr>     m_OptimizedGBuffer;
	ResourcePtr				m_DepthTarget;
	ResourcePtr				m_BackBuffer;

    MaterialDX11*           m_pMaterial;
    RenderEffectDX11*       m_pGBufferEffect;
    RenderEffectDX11*       m_pOptGBufferEffect;

    int                     m_iOptGbufferVS;
    int                     m_iOptGbufferPS;

    ResourcePtr             m_DiffuseTexture;
    ResourcePtr             m_NormalMap;

	ViewGBuffer*			m_pGBufferView;
    ViewLights*             m_pLightsView;
	SpriteRendererDX11		m_SpriteRenderer;
	SpriteFontDX11			m_Font;

	Node3D*					m_pNode;
	Entity3D*				m_pEntity;

	Camera*					m_pCamera;

	bool					m_bSaveScreenshot;

    // App Settings
    DisplayMode             m_DisplayMode;
    LightMode               m_LightMode;
    bool                    m_bEnableGBufferOpt;
};