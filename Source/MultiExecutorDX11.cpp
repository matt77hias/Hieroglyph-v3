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
#include "PCH.h"
#include "MultiExecutorDX11.h"
#include "RendererDX11.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MultiExecutorDX11::MultiExecutorDX11( )
{
}
//--------------------------------------------------------------------------------
MultiExecutorDX11::~MultiExecutorDX11()
{
}
//--------------------------------------------------------------------------------
void MultiExecutorDX11::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	// Propagate the execute call to all sub-executors.

	for ( auto pExecutor : m_Executors )
		pExecutor->Execute( pPipeline, pParamManager );
}
//--------------------------------------------------------------------------------
int MultiExecutorDX11::GetInputLayout( int ShaderID )
{
	// Since this executor doesn't actually ever get used, this method should
	// never be called!  If it is called, it means that someone sub-classed from
	// this class and is using it improperly.

	Log::Get().Write( L"ERROR: Trying to get the input layout of a MultiExecutorDX11!!!" );

	return( -1 );
}
//--------------------------------------------------------------------------------
void MultiExecutorDX11::GenerateInputLayout( int ShaderID )
{
	// Propagate the call to all sub-executors.

	for ( auto pExecutors : m_Executors )
		pExecutors->GenerateInputLayout( ShaderID );
}
//--------------------------------------------------------------------------------
void MultiExecutorDX11::SetLayoutElements( unsigned int count, D3D11_INPUT_ELEMENT_DESC* pElements )
{
	// Since this executor doesn't actually ever get used, this method should
	// never be called!  If it is called, it means that someone sub-classed from
	// this class and is using it improperly.

	Log::Get().Write( L"ERROR: Trying to set the layout elements of a MultiExecutorDX11!!!" );
}
//--------------------------------------------------------------------------------
void MultiExecutorDX11::AddExecutor( ExecutorPtr pExecutor )
{
	m_Executors.push_back( pExecutor );
}
//--------------------------------------------------------------------------------