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
// ParticleSystemEntity
//
//--------------------------------------------------------------------------------
#ifndef ParticleSystemEntity_h
#define ParticleSystemEntity_h
//--------------------------------------------------------------------------------
#include "Entity3D.h"
#include "ViewSimulation.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ParticleSystemEntity : public Entity3D
	{
	public:
		ParticleSystemEntity( );
		virtual ~ParticleSystemEntity( );

		virtual void PreRender( RendererDX11* pRenderer, VIEWTYPE view );
		virtual void Render( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager, VIEWTYPE view );


	public:
		ResourcePtr ArgsBuffer;
		ViewSimulation* m_pSimulation;

		ResourcePtr ParticleTexture;
		int			LinearSampler;
	};
};
//--------------------------------------------------------------------------------
#endif // ParticleSystemEntity_h
