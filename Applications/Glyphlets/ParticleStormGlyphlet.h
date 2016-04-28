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
// ParticleStormGlyphlet
//
// This glyphlet implements a duplication of the ParticleStorm demo, with the 
// exception that the program code is all running in a glyphlet instead of a 
// traditional RenderApplication instance.  In fact, the source files for the demo
// are directly referenced from their locations in the ParticleStorm project.
//--------------------------------------------------------------------------------
#ifndef ParticleStormGlyphlet_h
#define ParticleStormGlyphlet_h
//--------------------------------------------------------------------------------
#include "SingleWindowGlyphlet.h"
#include "VectorParameterDX11.h"
#include "ParticleSystemActor.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ParticleStormGlyphlet : public SingleWindowGlyphlet
	{
	public:
		ParticleStormGlyphlet( );
		virtual ~ParticleStormGlyphlet();

		virtual void Initialize();
		virtual void Update( float dt );
		virtual void Shutdown();

		virtual bool HandleEvent( EventPtr pEvent );
		virtual std::wstring GetName( );

	protected:

		ParticleSystemActor*	m_pActor;

		VectorParameterDX11*	m_pTimeFactors;
		VectorParameterDX11*	m_pEmitterLocation;
		VectorParameterDX11*	m_pConsumerLocation;
	};
};
//--------------------------------------------------------------------------------
#endif // ParticleStormGlyphlet_h
