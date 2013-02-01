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
#include "IRenderView.h"
#include "ShaderResourceParameterDX11.h"
#include "MatrixParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewParaboloidEnvMap : public IRenderView
	{
	public:
		ViewParaboloidEnvMap( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget );

		virtual void Update( float fTime );
		virtual void PreDraw( RendererDX11* pRenderer );
		virtual void Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );

		virtual void SetBackColor( Vector4f color );
		virtual void SetViewPort( DWORD x, DWORD y, DWORD w, DWORD h, float MinZ, float MaxZ );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );


		virtual ~ViewParaboloidEnvMap();

	protected:
		Vector4f		m_vColor;
		int				m_iViewport;

		ResourcePtr		m_RenderTarget;
		ResourcePtr		m_DepthTarget;

		ShaderResourceParameterDX11*	m_pParaboloidTextureParam;
		MatrixParameterDX11*			m_pParaboloidBasisParam;

		Matrix4f		m_ParaboloidBasis;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewParaboloidEnvMap_h
