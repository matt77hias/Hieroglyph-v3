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
// MatrixArrayParameterWriterDX11
//
//--------------------------------------------------------------------------------
#ifndef MatrixArrayParameterWriterDX11_h
#define MatrixArrayParameterWriterDX11_h
//--------------------------------------------------------------------------------
#include "ParameterWriter.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MatrixArrayParameterWriterDX11 : public ParameterWriter
	{
	public:
		MatrixArrayParameterWriterDX11();
		virtual ~MatrixArrayParameterWriterDX11();

		virtual void WriteParameter( IParameterManager* pParamMgr );
		void SetValue( Matrix4f* Value );
		void SetCount( int count );

	protected:
		Matrix4f*						m_pValue;
		int								m_iCount;
	};
};
//--------------------------------------------------------------------------------
#endif // ParameterWriter_h
//--------------------------------------------------------------------------------

