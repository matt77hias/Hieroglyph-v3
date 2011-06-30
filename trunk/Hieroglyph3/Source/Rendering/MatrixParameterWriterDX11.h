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
// MatrixParameterWriterDX11
//
//--------------------------------------------------------------------------------
#ifndef MatrixParameterWriterDX11_h
#define MatrixParameterWriterDX11_h
//--------------------------------------------------------------------------------
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MatrixParameterWriterDX11 : public ParameterWriter
	{
	public:
		MatrixParameterWriterDX11();
		virtual ~MatrixParameterWriterDX11();

		virtual void WriteParameter( IParameterManager* pParamMgr );
		void SetValue( Matrix4f& Value );

	protected:
		Matrix4f						m_Value;
	};
};
//--------------------------------------------------------------------------------
#endif // MatrixParameterWriterDX11_h
//--------------------------------------------------------------------------------

