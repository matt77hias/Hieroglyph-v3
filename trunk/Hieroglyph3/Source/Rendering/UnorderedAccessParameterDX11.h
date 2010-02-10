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
// UnorderedAccessParameterDX11
//
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
//--------------------------------------------------------------------------------
#ifndef UnorderedAccessParameterDX11_h
#define UnorderedAccessParameterDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class UnorderedAccessParameterDX11 : public RenderParameterDX11
	{
	public:
		UnorderedAccessParameterDX11();
		virtual ~UnorderedAccessParameterDX11();

		virtual void SetParameterData( void* pData );
		virtual ParameterType GetParameterType();
		int GetIndex();

	protected:
		int		m_iUnorderedAccessView;
	};
};
//--------------------------------------------------------------------------------
#endif // UnorderedAccessParameterDX11_h
//--------------------------------------------------------------------------------

