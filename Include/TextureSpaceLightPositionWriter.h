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
// TextureSpaceLightPositionWriter
//
// This parameter writer will read the current value of the 'LightPosition'
// vector parameter and calculate its position in texture space, according to the
// current world transform (its inverse actually) and the specified object space 
// to texture space transform.  It then writes the value to the 'LightPositionTS'
// vector parameter.
//--------------------------------------------------------------------------------
#ifndef TextureSpaceLightPositionWriter_h
#define TextureSpaceLightPositionWriter_h
//--------------------------------------------------------------------------------
#include "VectorParameterWriterDX11.h"
#include "Matrix4f.h"
#include "VectorParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class TextureSpaceLightPositionWriter : public VectorParameterWriterDX11
	{
	public:
		TextureSpaceLightPositionWriter();
		virtual ~TextureSpaceLightPositionWriter();

		void SetObjectToTextureSpaceXform( Matrix4f& xform );

		virtual void WriteParameter( IParameterManager* pParamMgr );

	protected:
		Matrix4f						ObjectToTextureSpaceMatrix;

		VectorParameterDX11*			m_pLightPositionParameter;
		MatrixParameterDX11*			m_pWorldMatrixParameter;
	};
};
//--------------------------------------------------------------------------------
#endif // TextureSpaceLightPositionWriter_h
//--------------------------------------------------------------------------------

