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
// ObjectSpaceCameraPositionWriter
//
// This parameter writer will read the current value of the 'CameraPosition'
// vector parameter and calculate its position in object space, according to the
// current world transform (its inverse actually).  It then writes the value to 
// the 'CameraPositionOS' vector parameter.
//--------------------------------------------------------------------------------
#ifndef ObjectSpaceCameraPositionWriter_h
#define ObjectSpaceCameraPositionWriter_h
//--------------------------------------------------------------------------------
#include "VectorParameterWriterDX11.h"
#include "Matrix4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ObjectSpaceCameraPositionWriter : public VectorParameterWriterDX11
	{
	public:
		ObjectSpaceCameraPositionWriter();
		virtual ~ObjectSpaceCameraPositionWriter();

		virtual void WriteParameter( IParameterManager* pParamMgr );

	protected:
		VectorParameterDX11*			m_pCameraPositionParameter;
		MatrixParameterDX11*			m_pWorldMatrixParameter;
	};
};
//--------------------------------------------------------------------------------
#endif // ObjectSpaceCameraPositionWriter_h
//--------------------------------------------------------------------------------

