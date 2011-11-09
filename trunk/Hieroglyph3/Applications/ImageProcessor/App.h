
#include "RenderApplication.h"

#include "GeometryDX11.h"
#include "MaterialDX11.h"
#include "Camera.h"
#include "Scene.h"

#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"

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

	// Texture Resources
	ResourcePtr				m_Texture[3];
	ResourcePtr				m_Intermediate;
	ResourcePtr				m_Output;

	Node3D*					m_pNode;
	Entity3D*				m_pEntity;

	RenderEffectDX11*		m_pBruteForceGaussian;
	RenderEffectDX11*		m_pSeparableGaussianX;
	RenderEffectDX11*		m_pSeparableGaussianY;
	RenderEffectDX11*		m_pCachedGaussianX;
	RenderEffectDX11*		m_pCachedGaussianY;

	RenderEffectDX11*		m_pBruteForceBilateral;
	RenderEffectDX11*		m_pSeparableBilateralX;
	RenderEffectDX11*		m_pSeparableBilateralY;

	ShaderResourceParameterDX11* m_pColorMapParameter;
	ShaderResourceParameterDX11* m_pInputParameter;
	UnorderedAccessParameterDX11* m_pOutputParameter;

	int						m_iAlgorithm;
	int						m_iImage;
};
