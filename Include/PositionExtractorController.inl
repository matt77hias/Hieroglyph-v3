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
template <typename T>
PositionExtractorController<T>::PositionExtractorController() :
	m_pWriter( nullptr )
{
}
//--------------------------------------------------------------------------------
template <typename T>
PositionExtractorController<T>::~PositionExtractorController()
{
}
//--------------------------------------------------------------------------------
template <typename T>
void PositionExtractorController<T>::Update( float fTime )
{
	// If there is an entity and a writer set, then 
	// set in this controller.

	if ( nullptr != m_pEntity && nullptr != m_pWriter )
	{
		Vector3f WorldPosition = m_pEntity->Transform.LocalPointToWorldSpace( m_pEntity->Transform.Position() );
		m_pWriter->SetValue( Vector4f( WorldPosition, 1.0f ) );
	}
}
//--------------------------------------------------------------------------------
template <typename T>
void PositionExtractorController<T>::SetParameterWriter( VectorParameterWriterDX11* pWriter )
{
	m_pWriter = pWriter;
}
//--------------------------------------------------------------------------------
