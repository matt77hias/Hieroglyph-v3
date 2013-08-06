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
#include "PositionExtractorController.h"
#include "Entity3D.h"
#include "Vector4f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PositionExtractorController::PositionExtractorController() :
	m_pWriter( nullptr )
{
}
//--------------------------------------------------------------------------------
PositionExtractorController::~PositionExtractorController()
{
}
//--------------------------------------------------------------------------------
void PositionExtractorController::Update( float fTime )
{
	// If there is an entity and a writer set, then 
	// set in this controller.

	if ( nullptr != m_pEntity && nullptr != m_pWriter )
	{
		Vector3f WorldPosition = m_pEntity->LocalPointToWorldSpace( m_pEntity->Position() );
		m_pWriter->SetValue( Vector4f( WorldPosition, 1.0f ) );
	}
}
//--------------------------------------------------------------------------------
void PositionExtractorController::SetParameterWriter( VectorParameterWriterDX11* pWriter )
{
	m_pWriter = pWriter;
}
//--------------------------------------------------------------------------------
