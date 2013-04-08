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
template <class TVertex, class TInstance>
DrawIndexedInstancedExecutorDX11<TVertex,TInstance>::DrawIndexedInstancedExecutorDX11( ) :
	m_uiStart( 0 ),
	m_uiCount( 0 )
{
	// Initialize our buffer to a reasonable size
	InstanceBuffer.SetMaxVertexCount( 1000 );
}
//--------------------------------------------------------------------------------
template <class TVertex, class TInstance>
DrawIndexedInstancedExecutorDX11<TVertex,TInstance>::~DrawIndexedInstancedExecutorDX11()
{
}
//--------------------------------------------------------------------------------
template <class TVertex, class TInstance>
void DrawIndexedInstancedExecutorDX11<TVertex,TInstance>::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	// With instanced rendering, we only proceed if there are instances there to 
	// render.

	if ( InstanceBuffer.GetVertexCount() > 0 ) {

		// Upload all the data to our D3D11 buffer resources.
		VertexBuffer.UploadVertexData( pPipeline );
		IndexBuffer.UploadIndexData( pPipeline );
		InstanceBuffer.UploadVertexData( pPipeline );
	
		pPipeline->InputAssemblerStage.ClearDesiredState();

		// Set the Input Assembler state, then perform the draw call.
		int layout = GetInputLayout( pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.ShaderProgram.GetState() );
		pPipeline->InputAssemblerStage.DesiredState.InputLayout.SetState( layout );
		pPipeline->InputAssemblerStage.DesiredState.PrimitiveTopology.SetState( m_ePrimType );

		pPipeline->InputAssemblerStage.DesiredState.VertexBuffers.SetState( 0, VertexBuffer.GetVertexBuffer()->m_iResource );
		pPipeline->InputAssemblerStage.DesiredState.VertexBufferStrides.SetState( 0, sizeof( TVertex ) );
		pPipeline->InputAssemblerStage.DesiredState.VertexBufferOffsets.SetState( 0, 0 );

		pPipeline->InputAssemblerStage.DesiredState.VertexBuffers.SetState( 1, InstanceBuffer.GetVertexBuffer()->m_iResource );
		pPipeline->InputAssemblerStage.DesiredState.VertexBufferStrides.SetState( 1, sizeof( TInstance ) );
		pPipeline->InputAssemblerStage.DesiredState.VertexBufferOffsets.SetState( 1, 0 );

		pPipeline->InputAssemblerStage.DesiredState.IndexBuffer.SetState( IndexBuffer.GetIndexBuffer()->m_iResource );
		pPipeline->InputAssemblerStage.DesiredState.IndexBufferFormat.SetState( DXGI_FORMAT_R32_UINT );
	
		pPipeline->ApplyInputResources();

		// Here we provide an index count and an instance count to the indexed 
		// instanced draw call.
		if ( m_uiCount == 0 ) {
			pPipeline->DrawIndexedInstanced( IndexBuffer.GetIndexCount(), InstanceBuffer.GetVertexCount(), 0, 0, 0 );
		} else {
			pPipeline->DrawIndexedInstanced( IndexBuffer.GetIndexCount(), m_uiCount, 0, 0, m_uiStart );
		}
	}
}
//--------------------------------------------------------------------------------
template <class TVertex, class TInstance>
void DrawIndexedInstancedExecutorDX11<TVertex,TInstance>::ResetGeometry()
{
	// Reset the instances here to prepare for the next drawing pass.  Then
	// allow the super class to reset its buffers as well.

	ResetInstances();
	DrawIndexedExecutorDX11::ResetGeometry();
}
//--------------------------------------------------------------------------------
template <class TVertex, class TInstance>
void DrawIndexedInstancedExecutorDX11<TVertex,TInstance>::ResetInstances()
{
	InstanceBuffer.ResetVertices();
	m_uiStart = 0;
	m_uiCount = 0;
}
//--------------------------------------------------------------------------------
template <class TVertex, class TInstance>
void DrawIndexedInstancedExecutorDX11<TVertex,TInstance>::AddInstance( const TInstance& data )
{
	InstanceBuffer.AddVertex( data );
}
//--------------------------------------------------------------------------------
template <class TVertex, class TInstance>
unsigned int DrawIndexedInstancedExecutorDX11<TVertex,TInstance>::GetInstanceCount()
{
	// The number of instances is directly the number of vertices in the 
	// per-instance vertex buffer.

	return( InstanceBuffer.GetVertexCount() );
}
//--------------------------------------------------------------------------------
template <class TVertex, class TInstance>
void DrawIndexedInstancedExecutorDX11<TVertex,TInstance>::SetInstanceRange( unsigned int start, unsigned int end )
{
	// Validate the data before accepting it.
	if ( start < end && end < InstanceBuffer.GetVertexCount() ) {
		m_uiStart = start;
		m_uiCount = end-start;
	} else {
		m_uiStart = 0;
		m_uiCount = 0;
	}
}
//--------------------------------------------------------------------------------
