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
using namespace Glyph3;
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
template <class T>
TConfiguration<T>::TConfiguration()
{
}
//--------------------------------------------------------------------------------
template <class T>
TConfiguration<T>::~TConfiguration()
{
}
//--------------------------------------------------------------------------------
template <class T>
void TConfiguration<T>::SetConfiguration(const T& config)
{
	m_NextConfig = config;
}
//--------------------------------------------------------------------------------
template <class T>
void TConfiguration<T>::ApplyConfiguration( )
{
	m_CurrentConfig = m_NextConfig;
}
//--------------------------------------------------------------------------------
template <class T>
const T& TConfiguration<T>::GetConfiguration( )
{
	return( m_CurrentConfig );
}
//--------------------------------------------------------------------------------
