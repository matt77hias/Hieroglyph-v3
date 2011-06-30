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
// SamplerParameterWriterDX11
//
//--------------------------------------------------------------------------------
#ifndef SamplerParameterWriterDX11_h
#define SamplerParameterWriterDX11_h
//--------------------------------------------------------------------------------
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SamplerParameterWriterDX11 : public ParameterWriter
	{
	public:
		SamplerParameterWriterDX11();
		virtual ~SamplerParameterWriterDX11();

		virtual void WriteParameter( IParameterManager* pParamMgr );
		void SetValue( int Value );

	protected:
		int						m_Value;
	};
};
//--------------------------------------------------------------------------------
#endif // SamplerParameterWriterDX11_h
//--------------------------------------------------------------------------------

