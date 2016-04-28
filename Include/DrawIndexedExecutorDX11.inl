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
template <class TVertex>
DrawIndexedExecutorDX11<TVertex>::DrawIndexedExecutorDX11( )
{
}
//--------------------------------------------------------------------------------
template <class TVertex>
DrawIndexedExecutorDX11<TVertex>::~DrawIndexedExecutorDX11()
{
}
//--------------------------------------------------------------------------------
template <class TVertex>
void DrawIndexedExecutorDX11<TVertex>::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	// Since we are doing indexed rendering, we only want to proceed if some
	// indices are available (otherwise no primitives would be generated).

	if ( IndexBuffer.GetElementCount() > 0 ) {

		// Upload the vertex and index data (only if they have been modified).
		VertexBuffer.UploadData( pPipeline );
		IndexBuffer.UploadData( pPipeline );
	
		pPipeline->InputAssemblerStage.ClearDesiredState();

		// Set the Input Assembler state, then perform the draw call.
		int layout = GetInputLayout( pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.ShaderProgram.GetState() );
		pPipeline->InputAssemblerStage.DesiredState.InputLayout.SetState( layout );
		pPipeline->InputAssemblerStage.DesiredState.PrimitiveTopology.SetState( m_ePrimType );
		pPipeline->InputAssemblerStage.DesiredState.VertexBuffers.SetState( 0, VertexBuffer.GetBuffer()->m_iResource );
		pPipeline->InputAssemblerStage.DesiredState.VertexBufferStrides.SetState( 0, sizeof( TVertex ) );
		pPipeline->InputAssemblerStage.DesiredState.VertexBufferOffsets.SetState( 0, 0 );
		pPipeline->InputAssemblerStage.DesiredState.IndexBuffer.SetState( IndexBuffer.GetBuffer()->m_iResource );

		pPipeline->ApplyInputResources();

		// Perform the indexed draw call, which only depends on the number of 
		// indices that are available.  The number of primitives generated will be
		// a function of how many indices and the primitive topology set above.
		pPipeline->DrawIndexed( IndexBuffer.GetElementCount(), 0, 0 );
	}
}
//--------------------------------------------------------------------------------
template <class TVertex>
void DrawIndexedExecutorDX11<TVertex>::ResetGeometry()
{
	// Reset the instances here to prepare for the next drawing pass.  Then
	// allow the super class to reset its buffers as well.

	ResetIndices();
	DrawExecutorDX11::ResetGeometry();
}
//--------------------------------------------------------------------------------
template <class TVertex>
void DrawIndexedExecutorDX11<TVertex>::ResetIndices()
{
	IndexBuffer.ResetData();
}
//--------------------------------------------------------------------------------
template <class TVertex>
void DrawIndexedExecutorDX11<TVertex>::AddIndex( const unsigned int index )
{
	// This method does not check the value that is passed as an index against
	// the number of vertices that are out there. User's are allowed to do what they
	// want with this data, but have to ensure that the indices are correct!

	IndexBuffer.AddElement( index );
}
//--------------------------------------------------------------------------------
template <class TVertex>
void DrawIndexedExecutorDX11<TVertex>::AddIndices( const unsigned int i1, const unsigned int i2 )
{
	AddIndex( i1 );
	AddIndex( i2 );
}
//--------------------------------------------------------------------------------
template <class TVertex>
void DrawIndexedExecutorDX11<TVertex>::AddIndices( const unsigned int i1, const unsigned int i2, const unsigned int i3 )
{
	AddIndex( i1 );
	AddIndex( i2 );
	AddIndex( i3 );
}
//--------------------------------------------------------------------------------
template <class TVertex>
unsigned int DrawIndexedExecutorDX11<TVertex>::GetIndexCount()
{
	return( IndexBuffer.GetElementCount() );
}
//--------------------------------------------------------------------------------
template <class TVertex>
unsigned int DrawIndexedExecutorDX11<TVertex>::GetPrimitiveCount()
{
	unsigned int count = 0;
	unsigned int referencedIndices = IndexBuffer.GetElementCount();

	switch ( m_ePrimType )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED:
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		count = referencedIndices; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		count = referencedIndices / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		count = referencedIndices - 1; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		count = referencedIndices / 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		count = referencedIndices - 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		count = referencedIndices / 4; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		count = referencedIndices - 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		count = referencedIndices / 6; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		count = ( referencedIndices - 3 ) / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
		count = referencedIndices; break;
	case D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 4; break;
	case D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 5; break;
	case D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 6; break;
	case D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 7; break;
	case D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 8; break;
	case D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 9; break;
	case D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 10; break;
	case D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 11; break;
	case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 12; break;
	case D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 13; break;
	case D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 14; break;
	case D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 15; break;
	case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 16; break;
	case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 17; break;
	case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 18; break;
	case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 19; break;
	case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 20; break;
	case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 21; break;
	case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 22; break;
	case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 23; break;
	case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 24; break;
	case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 25; break;
	case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 26; break;
	case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 27; break;
	case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 28; break;
	case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 29; break;
	case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 30; break;
	case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 31; break;
	case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 32; break;
	}

	return( count );
}
//--------------------------------------------------------------------------------
template <class TVertex>
void DrawIndexedExecutorDX11<TVertex>::SetMaxIndexCount( unsigned int count )
{
	IndexBuffer.SetMaxElementCount( count );
}
//--------------------------------------------------------------------------------
