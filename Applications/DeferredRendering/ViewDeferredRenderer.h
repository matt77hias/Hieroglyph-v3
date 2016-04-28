//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// ViewDeferredRenderer
//
// This class provides a subclass of the ViewPerspective render view, which
// implements a deferred rendering set of operations.  This is accomplished with
// the help of two sub-render views: ViewGBuffer and ViewLights.  ViewGBuffer is
// used to produce an appropriate set of geometry buffers, while ViewLights is 
// used to generate a render target with geometric representations of lights
// combined with the geometry buffer.
//
// To the application, this entire system appears only as a single perspective
// render view.  This makes the rendering system more modular and easy to use in
// the end application.
//--------------------------------------------------------------------------------
#ifndef ViewDeferredRenderer_h
#define ViewDeferredRenderer_h
//--------------------------------------------------------------------------------
#include "SceneRenderTask.h"
#include "ViewGBuffer.h"
#include "ViewLights.h"
#include "SpriteRendererDX11.h"
#include "SpriteFontLoaderDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewDeferredRenderer : public SceneRenderTask
	{
	public:
		ViewDeferredRenderer( RendererDX11& Renderer, ResourcePtr RenderTarget );
		virtual ~ViewDeferredRenderer();

		virtual void Update( float fTime );
		virtual void QueuePreTasks( RendererDX11* pRenderer );
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );

		virtual void SetEntity( Entity3D* pEntity );
		virtual void SetScene( Scene* pScene );

		virtual void SetViewMatrix( const Matrix4f& matrix );
		virtual void SetProjMatrix( const Matrix4f& matrix );
		
		void SetClipPlanes( float NearClip, float FarClip );
		void SetupViews();

		virtual std::wstring GetName();

	protected:

        int							m_iCurrentViewport;
		int							ResolutionX;
		int							ResolutionY;

        float						m_fNearClip;
        float						m_fFarClip;

		std::vector<ResourcePtr>	m_GBuffer[GBufferOptMode::NumSettings][AAMode::NumSettings];
		ResourcePtr					m_DepthTarget[AAMode::NumSettings];
		ResourcePtr					m_ReadOnlyDepthTarget[AAMode::NumSettings];
		ResourcePtr					m_FinalTarget[AAMode::NumSettings];
		int							m_iViewport[AAMode::NumSettings];
		ResourcePtr					m_ResolveTarget;
		ResourcePtr					m_BackBuffer;

		ViewGBuffer*				m_pGBufferView;
		ViewLights*					m_pLightsView;

		SpriteRendererDX11			m_SpriteRenderer;
		SpriteFontPtr				m_pFont;

	};
};
//--------------------------------------------------------------------------------
#endif // ViewDeferredRenderer_h
