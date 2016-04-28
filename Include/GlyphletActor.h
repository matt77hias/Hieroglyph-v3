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
// GlyphletActor
//
// The GlyphletActor is intended for use with any SingleWindowGlyphlet instance, 
// and will render a quad that displays the contents of that instance.  This 
// allows for a 'television' like access to a Glyphlet, and makes it easy to 
// congregate multiple different Glyphlets in a local area.
//--------------------------------------------------------------------------------
#ifndef GlyphletActor_h
#define GlyphletActor_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "GeometryActor.h"
#include "SingleWindowGlyphlet.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class GlyphletActor : public GeometryActor
	{
	public:
		GlyphletActor();
		virtual ~GlyphletActor();

		void SetGlyphlet( std::shared_ptr<SingleWindowGlyphlet> pGlyphlet );

	public:
		std::shared_ptr<SingleWindowGlyphlet>		m_pGlyphlet;
		ResourcePtr									m_RenderTexture;
	};
};
//--------------------------------------------------------------------------------
#endif // GlyphletActor_h
