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
#include "ConstantBufferDX11.h"
#include "PipelineManagerDX11.h"
#include "IParameterManager.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ConstantBufferDX11::ConstantBufferDX11( Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer )
{
	m_pBuffer = pBuffer;
	m_bAutoUpdate = true;
}
//--------------------------------------------------------------------------------
ConstantBufferDX11::~ConstantBufferDX11()
{
	// Buffer is released in the BufferDX11 destructor
}
//--------------------------------------------------------------------------------
ResourceType ConstantBufferDX11::GetType()
{
	return( RT_CONSTANTBUFFER );
}
//--------------------------------------------------------------------------------
void ConstantBufferDX11::AddMapping( ConstantBufferMapping& mapping )
{
	m_Mappings.push_back( mapping );
}
//--------------------------------------------------------------------------------
void ConstantBufferDX11::EmptyMappings( )
{
	m_Mappings.clear();
}
//--------------------------------------------------------------------------------
void ConstantBufferDX11::EvaluateMappings( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	// Test the index to ensure that it is a constant buffer.  If the method above returns
	// a non-null result, then this is a constant buffer.
	if ( m_pBuffer ) 
	{
		if ( GetAutoUpdate() )
		{
			// Check the parameters that go into this constant buffer, and see if they have
			// new values that need to be loaded into the buffer for this frame.  If not,
			// then we can safely skip the update of this buffer.

			bool doUpdate = false;

			for ( unsigned int j = 0; j < m_Mappings.size(); j++ ) {
				if ( m_Mappings[j].pParameter->GetValueID( pParamManager->GetID() ) 
					!= m_Mappings[j].valueID ) {
					doUpdate = true;
					break;
				}
			}

			if ( doUpdate ) {

				// Map the constant buffer into system memory.  We map the buffer 
				// with the discard write flag since we don't care what was in the
				// buffer already.

				D3D11_MAPPED_SUBRESOURCE resource = 
					pPipeline->MapResource( this, 0, D3D11_MAP_WRITE_DISCARD, 0 );

				// Update each variable in the constant buffer.  These variables are identified
				// by their type, and are currently allowed to be Vector4f, Matrix4f, or Matrix4f
				// arrays.  Additional types will be added as they are needed...

				for ( unsigned int j = 0; j < m_Mappings.size(); j++ )
				{
					RenderParameterDX11* pParam		= m_Mappings[j].pParameter;
					unsigned int offset				= m_Mappings[j].offset;
					unsigned int size				= m_Mappings[j].size;
					unsigned int elements			= m_Mappings[j].elements;
					unsigned int valueID			= m_Mappings[j].valueID;
					unsigned int threadID			= pParamManager->GetID();


					m_Mappings[j].valueID = pParam->GetValueID( threadID );

					if ( m_Mappings[j].varclass == D3D_SVC_VECTOR )
					{
						Vector4f vector = pParamManager->GetVectorParameter( pParam );
						Vector4f* pBuf = (Vector4f*)((char*)resource.pData + offset);
						*pBuf = vector;
					}
					else if ( ( m_Mappings[j].varclass == D3D_SVC_MATRIX_ROWS ) ||
						( m_Mappings[j].varclass == D3D_SVC_MATRIX_COLUMNS ) )
					{
						// Check if it is an array of matrices first...
						if ( elements == 0 ) 
						{
							Matrix4f matrix = pParamManager->GetMatrixParameter( pParam );
							Matrix4f* pBuf = (Matrix4f*)((char*)resource.pData + offset);
							*pBuf = matrix;
						}
						else 
						{
							// If a matrix array, then use the corresponding parameter type.
							if ( size == elements * sizeof( Matrix4f ) ) {
								Matrix4f* pMatrices = pParamManager->GetMatrixArrayParameter( pParam );
								memcpy( ((char*)resource.pData + offset), (char*)pMatrices, size );
							} else {
								Log::Get().Write( L"Mismatch in matrix array count, update will not be performed!!!" );
							}
						}
					} else {
						Log::Get().Write( L"Non vector or matrix parameter specified in a constant buffer!  This will not be updated!" );
					}
				}

				pPipeline->UnMapResource( this, 0 );
			}
		}
	} else {
		Log::Get().Write( L"Trying to update a constant buffer that isn't a constant buffer!" );
	}
}
//--------------------------------------------------------------------------------
bool ConstantBufferDX11::ContainsMapping( int ID, const ConstantBufferMapping& mapping )
{
	bool result = false;

	// First retrieve the internal mapping

	unsigned int index = static_cast<unsigned int>( ID );

	if ( index < m_Mappings.size() ) {
		ConstantBufferMapping internalMapping = m_Mappings[index];

		if ( internalMapping.pParameter == mapping.pParameter 
			&& internalMapping.offset == mapping.offset 
			&& internalMapping.size == mapping.size
			&& internalMapping.varclass == mapping.varclass
			&& internalMapping.elements == mapping.elements ) {
			result = true;
		}
	}

	return( result );
}
//--------------------------------------------------------------------------------
void ConstantBufferDX11::SetAutoUpdate( bool enable )
{
	m_bAutoUpdate = enable;
}
//--------------------------------------------------------------------------------
bool ConstantBufferDX11::GetAutoUpdate()
{
	return( m_bAutoUpdate );
}
//--------------------------------------------------------------------------------