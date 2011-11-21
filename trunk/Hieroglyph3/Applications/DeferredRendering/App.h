
#include "RenderApplication.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"

#include "AppSettings.h"

using namespace Glyph3;

class App : public RenderApplication
{

public:
	App();

public:
	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual bool ConfigureRenderingSetup();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool HandleEvent( IEvent* pEvent );
	virtual std::wstring GetName( );

protected:


    void DrawHUD();

    MaterialPtr		        m_pMaterial;
    RenderEffectDX11*       m_pGBufferEffect[GBufferOptMode::NumSettings];
    int					    m_iGBufferDSState;
    int                     m_iGBufferRSState;

    ResourcePtr             m_DiffuseTexture;
    ResourcePtr             m_NormalMap;

	Node3D*					m_pNode;
	Entity3D*				m_pEntity;
};