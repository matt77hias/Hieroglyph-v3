
#include "Application.h"
#include "IEventListener.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "VertexElementDX11.h"
#include "GeometryDX11.h"
#include "RenderEffectDX11.h"

#include "SpriteRendererDX11.h"
#include "SpriteFontDX11.h"

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

	RendererDX11*				m_pRenderer11;
	Win32RenderWindow*			m_pWindow;

	bool						m_bSaveScreenshot;
	bool						m_bViewPointInAutoMode;
	bool						m_bSolidRender;
	bool						m_bSimpleComplexity;
	
	int							m_iSwapChain;
	ResourcePtr					m_RenderTarget;
	ResourcePtr					m_DepthTarget;

	SpriteFontDX11*				m_pFont;
	SpriteRendererDX11*			m_pSpriteRenderer;

	GeometryDX11*				m_pTerrainGeometry;
	RenderEffectDX11*			m_pTerrainEffect;
	ResourcePtr					m_pHeightMapTexture;
	RenderEffectDX11*			m_pComputeShaderEffect;
	ResourcePtr					m_pLodLookupTexture;

	int							m_rsWireframe;
	int							m_rsSolid;

	static const int			TERRAIN_X_LEN = 32;
	static const int			TERRAIN_Z_LEN = 32;

	enum ShadingMode
	{
		SolidColour,
		SimpleShading,
		LodDebugView
	};

	ShadingMode					m_smCurrentShading;
	std::map<ShadingMode, int>	m_TerrainDomainShaders;

	int							m_iComplexHullShader;
	int							m_iSimpleHullShader;

	virtual void CreateTerrainGeometry();
	virtual void CreateTerrainShaders();
	virtual void CreateTerrainTextures();
	virtual void UpdateViewState();
	virtual void CreateComputeShaderResources();
	virtual void RunComputeShader();
};
