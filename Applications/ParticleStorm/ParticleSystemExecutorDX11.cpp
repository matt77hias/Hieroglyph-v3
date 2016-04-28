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
#include "ParticleSystemExecutorDX11.h"
#include "IndirectArgsBufferDX11.h"
#include "Log.h"
#include "GlyphString.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ParticleSystemExecutorDX11::ParticleSystemExecutorDX11( )
{
}
//--------------------------------------------------------------------------------
ParticleSystemExecutorDX11::~ParticleSystemExecutorDX11()
{
}
//--------------------------------------------------------------------------------
void ParticleSystemExecutorDX11::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	if ( nullptr != ArgumentsBuffer ) {

		IndirectArgsBufferDX11* pBuffer = RendererDX11::Get()->GetIndirectArgsBufferByIndex( ArgumentsBuffer->m_iResource );

		if ( !pBuffer ) {
			Log::Get().Write( L"Failure to get indirect args buffer, aborting call..." );
			return;
		} 

		ID3D11Buffer* pArgsBuffer = reinterpret_cast<ID3D11Buffer*>( pBuffer->GetResource() );

		pPipeline->InputAssemblerStage.ClearDesiredState();

		// Set the Input Assembler state, then perform the draw call.
		int layout = GetInputLayout( pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.ShaderProgram.GetState() );
		pPipeline->InputAssemblerStage.DesiredState.InputLayout.SetState( layout );
		pPipeline->InputAssemblerStage.DesiredState.PrimitiveTopology.SetState( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

		pPipeline->ApplyInputResources();

		pPipeline->DrawInstancedIndirect( pArgsBuffer, 0 );
	}
}
//--------------------------------------------------------------------------------
void ParticleSystemExecutorDX11::SetArgumentsBuffer( ResourcePtr buffer )
{
	// Reset the vertex and index count here to prepare for the next drawing pass.

	ArgumentsBuffer = buffer;
}
//--------------------------------------------------------------------------------
