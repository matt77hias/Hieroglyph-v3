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
// SceneRenderTask
//
// A scene render task is intended to take a scene and perform some rendering into
// render targets.  The type of render targets is not imposed by this class, but
// the normal case is that there will be a render target and a depth/stencil 
// target.  If a special rendering scenario is needed, then different types of
// targets can be used.
// 
// This class extends the abstract 'Task' base class, and thus
// requires any subclasses to implement the Task interfaces.  This allows all
// subclasses to be usable within the material system as a dependency for a 
// material.  This is helpful in cases where a material requires some product of
// a scene, such as an environment map.
//--------------------------------------------------------------------------------
#ifndef SceneRenderTask_h
#define SceneRenderTask_h
//--------------------------------------------------------------------------------
#include "Task.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;
	class Scene;
	class BoundsVisualizerActor;

	// The view type is used to allow a view to identify what type of
	// view it is.  This identifier is also used by objects to specify
	// special rendering effects to use in that particular render view.

	enum VIEWTYPE
	{
		VT_INVALID,
		VT_PERSPECTIVE,
		VT_LINEAR_DEPTH_NORMAL,
		VT_DUAL_PARABOLOID_ENVMAP,
		VT_GBUFFER,
		VT_LIGHTS,
        VT_FINALPASS,
		VT_SILHOUETTE,
		VT_NUM_VIEW_TYPES
	};    

	class SceneRenderTask : public Task
	{
	public:

		SceneRenderTask( );
		virtual ~SceneRenderTask( );

		// We assume that a scene rendering will eventually output to a rectangular
		// output resource.  In this case, if the output should be resized, then
		// the subclasses have to implement a resize method.

		virtual void Resize( UINT width, UINT height ) = 0;

		// This class introduces a view and projection matrix as parameters, so
		// they are bound in their respective 'render' and 'usage' interfaces.

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );

		// A reference is provided to allow views to reference it's entity's data.

		virtual void SetEntity( Entity3D* pEntity );

		// A reference to the scene that will be rendered is provided.

		virtual void SetScene( Scene* pScene );

		// The viewports are specified through these interfaces. Traditionally you would
		// only use a single viewport, but it is possible to use multiples when using a 
		// geometry shader and outputting a SV_ViewportArrayIndex semantic attribute. The
		// functions assume a single viewport unless otherwise specified.

		void SetViewPort( int viewport, unsigned int index = 0 );
		void SetViewPortCount( unsigned int count );
		void ConfigureViewports( PipelineManagerDX11* pPipeline );
		const ViewPortDX11& GetViewPort( unsigned int index );


		virtual void SetBackColor( const Vector4f& color );

		virtual void SetViewMatrix( const Matrix4f& matrix );
		virtual void SetProjMatrix( const Matrix4f& matrix );
		Matrix4f GetViewMatrix( );
		Matrix4f GetProjMatrix( );

		void SetDebugViewEnabled( bool debug );
		bool IsDebugViewEnabled();

	protected:

		Entity3D* m_pEntity;
		Scene* m_pScene;

		BoundsVisualizerActor* m_pDebugVisualizer;

		Vector4f		m_vColor;
		int				m_iViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		unsigned int	m_uiViewportCount;

		Matrix4f ViewMatrix;
		Matrix4f ProjMatrix;

		bool m_bDebugViewEnabled;
	};
};
//--------------------------------------------------------------------------------
#endif // SceneRenderTask_h
