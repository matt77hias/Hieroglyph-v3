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
// UnorderedAccessParameterDX11
//
//--------------------------------------------------------------------------------
#ifndef UnorderedAccessParameterDX11_h
#define UnorderedAccessParameterDX11_h
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct UAVParameterData
	{
		int				m_iUnorderedAccessView;
		unsigned int	m_iInitialCount;
	};

	class UnorderedAccessParameterDX11 : public RenderParameterDX11
	{
	public:
		UnorderedAccessParameterDX11();
		UnorderedAccessParameterDX11( UnorderedAccessParameterDX11& copy );
		virtual ~UnorderedAccessParameterDX11();

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 );
		//virtual void ResetParameterData( void* pData, unsigned int threadID = 0 );

		virtual const ParameterType GetParameterType();
		int GetIndex( unsigned int threadID );
		unsigned int GetInitialCount( unsigned int threadID );

		//void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 );

	protected:
		UAVParameterData m_ParameterData[NUM_THREADS+1];
	};
};
//--------------------------------------------------------------------------------
#endif // UnorderedAccessParameterDX11_h
//--------------------------------------------------------------------------------

