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
// PointLight
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef PointLight_h
#define PointLight_h
//--------------------------------------------------------------------------------
#include "Light.h"
#include "PositionExtractorController.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PointLight : public Light
	{
	public:
		PointLight();
		virtual ~PointLight();

	protected:
		VectorParameterWriterDX11* m_PositionWriter;
	};
};
//--------------------------------------------------------------------------------
#endif // PointLight_h
//--------------------------------------------------------------------------------
