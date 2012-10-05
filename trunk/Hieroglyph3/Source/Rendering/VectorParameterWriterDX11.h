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
// VectorParameterWriterDX11
//
//--------------------------------------------------------------------------------
#ifndef VectorParameterWriterDX11_h
#define VectorParameterWriterDX11_h
//--------------------------------------------------------------------------------
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VectorParameterWriterDX11 : public ParameterWriter
	{
	public:
		VectorParameterWriterDX11();
		virtual ~VectorParameterWriterDX11();

		void SetRenderParameterRef( VectorParameterDX11* pParam );

		virtual void WriteParameter( IParameterManager* pParamMgr );
		void SetValue( Vector4f& Value );

	protected:
		Vector4f						m_Value;
	};
};
//--------------------------------------------------------------------------------
#endif // VectorParameterWriterDX11_h
//--------------------------------------------------------------------------------

