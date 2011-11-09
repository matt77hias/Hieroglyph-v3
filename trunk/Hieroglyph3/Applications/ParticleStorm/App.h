
#include "RenderApplication.h"

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

class App : public RenderApplication
{

public:
	App();
	
public:
	virtual bool ConfigureEngineComponents();
	virtual void ShutdownEngineComponents();

	virtual void Initialize();
	virtual void Update();
	virtual void Shutdown();

	virtual bool HandleEvent( IEvent* pEvent );
	virtual std::wstring GetName( );

protected:

	Node3D*					m_pNode;
	Entity3D*				m_pEntity;

	VectorParameterDX11*	m_pTimeFactors;
	VectorParameterDX11*	m_pEmitterLocation;
	VectorParameterDX11*	m_pConsumerLocation;
};
