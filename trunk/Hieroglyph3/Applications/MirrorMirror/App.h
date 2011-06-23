
#include "RenderApplication.h"

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
#include "VectorParameterDX11.h"

using namespace Glyph3;

class App : public RenderApplication
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

	Node3D*					m_pNode;
	DiffuseSphereEntity*	m_pEntity;
	ReflectiveSphereEntity*	m_pReflector[16];
	
	Actor*					m_pDiffuseActor;
	Actor*					m_pReflectiveActor;

	Vector4f				m_LightParams;
	Vector4f				m_LightPosition;

	VectorParameterDX11*	m_pLightColor;
	VectorParameterDX11*	m_pLightPosition;
	VectorParameterDX11*	m_pTimeFactors;
	VectorParameterDX11*	m_pViewPosition;
};
