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
// RasterizerStageStateDX11
//
//--------------------------------------------------------------------------------
#ifndef RasterizerStageStateDX11_h
#define RasterizerStageStateDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class RasterizerStageDX11;

	class RasterizerStageStateDX11
	{
	public:
		RasterizerStageStateDX11();
		virtual ~RasterizerStageStateDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );

		void SetRasterizerState( int state );
		void SetViewportCount( int count );
		void SetViewport( unsigned int slot, int viewport );
		void SetScissorRectCount( int count );
		void SetScissorRect( unsigned int slot, D3D11_RECT& rect );
		
		int GetRasterizerState() const;
		int GetViewportCount() const;
		int GetViewport( UINT slot ) const;
		int GetScissorRectCount() const;
		D3D11_RECT GetScissorRect( UINT slot ) const;

		int CompareRasterizerState( RasterizerStageStateDX11& desired );
		int CompareViewportState( RasterizerStageStateDX11& desired );
		int CompareScissorRectState( RasterizerStageStateDX11& desired );

		void ClearState( );

		void SetSisterState( RasterizerStageStateDX11* pState );
		void ResetUpdateFlags( );

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		int								RasterizerState;
		int								ViewportCount;
		int								Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		int								ScissorRectCount;
		D3D11_RECT						ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		
		RasterizerStageStateDX11*		m_pSisterState;

		bool							m_bUpdateRasterizerState;
		bool							m_bUpdateViewportState;
		bool							m_bUpdateScissorRectState;

		friend RasterizerStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // RasterizerStageStateDX11_h
//--------------------------------------------------------------------------------

