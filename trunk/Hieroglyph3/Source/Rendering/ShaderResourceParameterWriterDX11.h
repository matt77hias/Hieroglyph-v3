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
// ShaderResourceParameterWriterDX11
//
//--------------------------------------------------------------------------------
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
#ifndef ShaderResourceParameterWriterDX11_h
#define ShaderResourceParameterWriterDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ShaderResourceParameterWriterDX11 : public ParameterWriter
	{
	public:
		ShaderResourceParameterWriterDX11();
		virtual ~ShaderResourceParameterWriterDX11();

		virtual void WriteParameter( IParameterManager* pParamMgr );
		void SetValue( ResourcePtr Value );

	protected:
		ResourcePtr						m_Value;
	};
};
//--------------------------------------------------------------------------------
#endif // ShaderResourceParameterWriterDX11_h
//--------------------------------------------------------------------------------

