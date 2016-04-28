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
IController<T>::IController()
{
	m_pEntity = 0;
}
//--------------------------------------------------------------------------------
template <typename T>
IController<T>::~IController()
{
}
//--------------------------------------------------------------------------------
template <typename T>
void IController<T>::SetEntity( T* pEntity )
{
	m_pEntity = pEntity;
}
//--------------------------------------------------------------------------------
template <typename T>
T* IController<T>::GetEntity()
{
	return( m_pEntity );
}
//--------------------------------------------------------------------------------