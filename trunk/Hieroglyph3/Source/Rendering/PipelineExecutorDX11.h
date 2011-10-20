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
// PipelineExecutorDX11
//
//--------------------------------------------------------------------------------
#ifndef PipelineExecutorDX11_h
#define PipelineExecutorDX11_h
//--------------------------------------------------------------------------------
#include <memory>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PipelineManagerDX11;
	class IParameterManager;

	class PipelineExecutorDX11
	{
	public:
		PipelineExecutorDX11();
		virtual ~PipelineExecutorDX11();

		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager ) = 0;
	};

	typedef std::shared_ptr<PipelineExecutorDX11> ExecutorPtr;
};
//--------------------------------------------------------------------------------
#endif // PipelineExecutorDX11_h
//--------------------------------------------------------------------------------

