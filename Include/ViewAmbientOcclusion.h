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
// ViewAmbientOcclusion
//
// This class extends the standard perspective view to produce a depth/normal 
// buffer, then produce an occlusion buffer from it, and then finally produce an
// end image like a perspective view would.
//--------------------------------------------------------------------------------
#ifndef ViewAmbientOcclusion_h
#define ViewAmbientOcclusion_h
//--------------------------------------------------------------------------------
#include "ViewPerspective.h"
#include "ViewOcclusion.h"
#include "ViewDepthNormal.h"
#include "Actor.h"
#include "ShaderResourceParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewAmbientOcclusion : public ViewPerspective
	{
	public:
		ViewAmbientOcclusion( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget = 0 );
		virtual ~ViewAmbientOcclusion();

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

		void SetVisualizationActive( bool active );

		virtual std::wstring GetName();

	protected:

		int ResolutionX;
		int ResolutionY;

		ResourcePtr DepthNormalBuffer;
		ResourcePtr OcclusionBuffer;
		ResourcePtr BilateralBuffer;

		ViewOcclusion*		pOcclusionView;
		ViewDepthNormal*	pDepthNormalView;

		Actor*				pVisActor;

		ShaderResourceParameterDX11*	pDepthBufferParameter;
		ShaderResourceParameterDX11*	pOcclusionBufferParameter;

		bool				bRenderVisualization;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewAmbientOcclusion_h
