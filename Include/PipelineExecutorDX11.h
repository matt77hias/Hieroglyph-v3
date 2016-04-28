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
// PipelineExecutorDX11
//
//--------------------------------------------------------------------------------
#ifndef PipelineExecutorDX11_h
#define PipelineExecutorDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PipelineManagerDX11;
	class IParameterManager;

	// This simple structure provides the mapping between a vertex shader program
	// and the corresponding input layout.

	struct InputLayoutKey
	{
		int shader;
		int layout;
	};

	class PipelineExecutorDX11
	{
	public:
		PipelineExecutorDX11();
		virtual ~PipelineExecutorDX11();

		virtual void Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager ) = 0;
		
		virtual void SetLayoutElements( unsigned int count, D3D11_INPUT_ELEMENT_DESC* pElements );
		virtual void GenerateInputLayout( int ShaderID );
		virtual int GetInputLayout( int ShaderID );

	protected:

		// A description of our vertex elements
		std::vector<D3D11_INPUT_ELEMENT_DESC>	m_elements;
		std::map<int,InputLayoutKey*>			m_InputLayouts;

	};

	typedef std::shared_ptr<PipelineExecutorDX11> ExecutorPtr;
};
//--------------------------------------------------------------------------------
#endif // PipelineExecutorDX11_h
//--------------------------------------------------------------------------------

