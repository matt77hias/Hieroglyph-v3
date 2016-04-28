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
// ParticleSystemActor
//
//--------------------------------------------------------------------------------
#ifndef ParticleSystemActor_h
#define ParticleSystemActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "ParticleSystemExecutorDX11.h"
#include "ViewSimulation.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ParticleSystemActor : public Actor
	{
	public:
		ParticleSystemActor();
		virtual ~ParticleSystemActor();

	protected:
		ParticleSystemExecutorPtr				m_pExecutor;
		MaterialPtr								m_pMaterial;

		ViewSimulation*							m_pSimulation;
		ResourcePtr								ParticleTexture;
		int										LinearSampler;

	};
};
//--------------------------------------------------------------------------------
#endif // ParticleSystemActor_h
