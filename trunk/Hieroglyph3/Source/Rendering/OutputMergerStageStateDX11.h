//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// OutputMergerStageStateDX11
//
//--------------------------------------------------------------------------------
#ifndef OutputMergerStageStateDX11_h
#define OutputMergerStageStateDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class OutputMergerStageDX11;

	class OutputMergerStageStateDX11
	{
	public:
		OutputMergerStageStateDX11();
		virtual ~OutputMergerStageStateDX11();

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

		int CompareRasterizerState( OutputMergerStageStateDX11& desired );
		int CompareViewportState( OutputMergerStageStateDX11& desired );
		int CompareScissorRectState( OutputMergerStageStateDX11& desired );

		void ClearState( );

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		int								RasterizerState;
		int								ViewportCount;
		int								Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		int								ScissorRectCount;
		D3D11_RECT						ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		
		friend OutputMergerStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // OutputMergerStageStateDX11_h
//--------------------------------------------------------------------------------

