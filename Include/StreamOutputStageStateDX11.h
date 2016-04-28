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
// StreamOutputStageStateDX11
//
//--------------------------------------------------------------------------------
#ifndef StreamOutputStageStateDX11_h
#define StreamOutputStageStateDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "TStateMonitor.h"
#include "TStateArrayMonitor.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class StreamOutputStageStateDX11
	{
	public:
		StreamOutputStageStateDX11();
		virtual ~StreamOutputStageStateDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );
		void ClearState( );
		void SetSisterState( StreamOutputStageStateDX11* pState );
		void ResetUpdateFlags( );

		int GetBufferCount() const;

		TStateArrayMonitor< int, 4 >  StreamBuffers;
		TStateArrayMonitor< unsigned int, 4 > StreamOffsets;

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		StreamOutputStageStateDX11*		m_pSisterState;
	};
};
//--------------------------------------------------------------------------------
#endif // StreamOutputStageStateDX11_h
//--------------------------------------------------------------------------------

