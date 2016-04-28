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
// ViewParaboloidEnvMap
//
//--------------------------------------------------------------------------------
#ifndef ViewParaboloidEnvMap_h
#define ViewParaboloidEnvMap_h
//--------------------------------------------------------------------------------
#include "SceneRenderTask.h"
#include "ShaderResourceParameterDX11.h"
#include "MatrixParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewParaboloidEnvMap : public SceneRenderTask
	{
	public:
		ViewParaboloidEnvMap( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget );
		virtual ~ViewParaboloidEnvMap();

		virtual void Update( float fTime );
		virtual void QueuePreTasks( RendererDX11* pRenderer );
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );

		void SetMaxRecurrence( int max );
		int GetMaxRecurrence( );
		void ResetRecurrence( );

		virtual std::wstring GetName();
		

	protected:
		ResourcePtr		m_RenderTarget;
		ResourcePtr		m_DepthTarget;

		ShaderResourceParameterDX11*	m_pParaboloidTextureParam;
		MatrixParameterDX11*			m_pParaboloidBasisParam;

		Matrix4f		m_ParaboloidBasis;

		int		m_iMaxRecurrence;
		int		m_iCurrRecurrence;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewParaboloidEnvMap_h
