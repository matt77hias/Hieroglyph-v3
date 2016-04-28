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
// ViewLightPrepassRenderer
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
#ifndef ViewLightPrepassRenderer_h
#define ViewLightPrepassRenderer_h
//--------------------------------------------------------------------------------
#include "SceneRenderTask.h"
#include "ViewGBuffer.h"
#include "ViewLights.h"
#include "ViewFinalPass.h"
#include "SpriteRendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewLightPrepassRenderer : public SceneRenderTask
	{
	public:
		ViewLightPrepassRenderer( RendererDX11& Renderer, ResourcePtr RenderTarget );
		virtual ~ViewLightPrepassRenderer();

		virtual void Update( float fTime );
		virtual void QueuePreTasks( RendererDX11* pRenderer );
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

		virtual void SetEntity( Entity3D* pEntity );
		virtual void SetScene( Scene* pScene );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );

		virtual void SetViewMatrix( const Matrix4f& matrix );
		virtual void SetProjMatrix( const Matrix4f& matrix );
		
		void SetClipPlanes( float NearClip, float FarClip );
		void SetupLights();

		virtual std::wstring GetName();

	protected:

		int						ResolutionX;
		int						ResolutionY;

        float					m_fNearClip;
        float					m_fFarClip;

		ResourcePtr     		m_GBufferTarget;
		ResourcePtr             m_LightTarget;
		ResourcePtr				m_DepthTarget;
		ResourcePtr             m_ReadOnlyDepthTarget;
		ResourcePtr             m_FinalTarget;
		ResourcePtr             m_ResolveTarget;

		ResourcePtr				m_BackBuffer;

		ViewGBuffer*			m_pGBufferView;
		ViewLights*             m_pLightsView;
		ViewFinalPass*          m_pFinalPassView;

		SpriteRendererDX11		m_SpriteRenderer;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewLightPrepassRenderer_h
