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
// TextureSpaceCameraPositionWriter
//
// This parameter writer will read the current value of the 'CameraPosition'
// vector parameter and calculate its position in texture space, according to the
// current world transform (its inverse actually) and the specified object space 
// to texture space transform.  It then writes the value to the 'CameraPositionTS'
// vector parameter.
//--------------------------------------------------------------------------------
#ifndef TextureSpaceCameraPositionWriter_h
#define TextureSpaceCameraPositionWriter_h
//--------------------------------------------------------------------------------
#include "VectorParameterWriterDX11.h"
#include "Matrix4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class TextureSpaceCameraPositionWriter : public VectorParameterWriterDX11
	{
	public:
		TextureSpaceCameraPositionWriter();
		virtual ~TextureSpaceCameraPositionWriter();

		void SetObjectToTextureSpaceXform( Matrix4f& xform );

		virtual void WriteParameter( IParameterManager* pParamMgr );

	protected:
		Matrix4f						ObjectToTextureSpaceMatrix;

		VectorParameterDX11*			m_pCameraPositionParameter;
		MatrixParameterDX11*			m_pWorldMatrixParameter;
	};
};
//--------------------------------------------------------------------------------
#endif // TextureSpaceCameraPositionWriter_h
//--------------------------------------------------------------------------------

