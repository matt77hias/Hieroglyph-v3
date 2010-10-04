
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

    enum DisplayMode 
    {
        Final = 0, 
        GBuffer = 1, 
        Normals = 2,
        Position = 3,
        DiffuseAlbedo = 4,
        SpecularAlbedo = 5,

        NumDisplayModes
    };

	RendererDX11*			m_pRenderer11;
	Win32RenderWindow*		m_pWindow;

	TArray<ResourcePtr>		m_GBufferTargets;    
	ResourcePtr				m_DepthTarget;
	ResourcePtr				m_BackBuffer;

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

    DisplayMode             m_DisplayMode;
};
