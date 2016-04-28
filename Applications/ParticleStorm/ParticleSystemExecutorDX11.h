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
// ParticleSystemExecutorDX11
//
//--------------------------------------------------------------------------------
#ifndef ParticleSystemExecutorDX11_h
#define ParticleSystemExecutorDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "PipelineExecutorDX11.h"
#include "ResourceProxyDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ParticleSystemExecutorDX11 : public PipelineExecutorDX11
	{
	public:
		ParticleSystemExecutorDX11( );
		virtual ~ParticleSystemExecutorDX11( );
	
		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );

		void SetArgumentsBuffer( ResourcePtr buffer );

	protected:
		
		ResourcePtr ArgumentsBuffer;
	};

	typedef std::shared_ptr<ParticleSystemExecutorDX11> ParticleSystemExecutorPtr;
};
//--------------------------------------------------------------------------------
#endif // ParticleSystemExecutorDX11_h
