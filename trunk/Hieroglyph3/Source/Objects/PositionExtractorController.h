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
// PositionExtractorController
//
//--------------------------------------------------------------------------------
#ifndef PositionExtractorController_h
#define PositionExtractorController_h
//--------------------------------------------------------------------------------
#include "IController.h"
#include "VectorParameterWriterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PositionExtractorController : public IController
	{
	public:
		PositionExtractorController( );
		virtual ~PositionExtractorController( );

		virtual void Update( float fTime );

		void SetParameterWriter( VectorParameterWriterDX11* pWriter );

	protected:
		VectorParameterWriterDX11*		m_pWriter;
	};
};
//--------------------------------------------------------------------------------
#endif // PositionExtractorController_h
