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
// MultiExecutorDX11
//
// This pipeline executor is actually just a container for multiple other 
// pipeline executors.  The use of this object allows a single Entity3D instance
// to utilize more than one draw call for each 'Render' call to that entity.
//
// This is primarily intended to allow multiple types of draw calls and/or 
// multiple different input topologies to be rendered at the same time with the
// same material.  If you want to render the same geometry multiple times but with
// different pipeline configurations, then you should utilize a modified 
// MaterialDX11 instead.
//--------------------------------------------------------------------------------
#ifndef MultiExecutorDX11_h
#define MultiExecutorDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "PipelineExecutorDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PipelineManagerDX11;
	class IParameterManager;

	class MultiExecutorDX11 : public PipelineExecutorDX11
	{
	public:
		MultiExecutorDX11();
		virtual ~MultiExecutorDX11();

		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );
		
		virtual void SetLayoutElements( unsigned int count, D3D11_INPUT_ELEMENT_DESC* pElements );
		virtual void GenerateInputLayout( int ShaderID );
		virtual int GetInputLayout( int ShaderID );

		void AddExecutor( ExecutorPtr pExecutor );

	protected:
		std::vector<ExecutorPtr> m_Executors;
	};

	typedef std::shared_ptr<MultiExecutorDX11> MultiExecutorPtr;
};
//--------------------------------------------------------------------------------
#endif // MultiExecutorDX11_h
//--------------------------------------------------------------------------------

