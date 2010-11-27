//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
template <class T>
TArray<T>::TArray()
{
	m_iQuantity = 0;
	m_iCapacity = 5;

	m_aData = new T[m_iCapacity];
}
//--------------------------------------------------------------------------------
template <class T>
TArray<T>::TArray(const TArray& item)
{
	m_aData = NULL;
	*this = item;
}
//--------------------------------------------------------------------------------
template <class T>
TArray<T>::~TArray()
{
	delete[] m_aData;
}
//--------------------------------------------------------------------------------
template <class T>
TArray<T>& TArray<T>::operator=(const TArray& item)
{
	m_iQuantity = item.m_iQuantity;
	m_iCapacity = item.m_iCapacity;

	delete[] m_aData;
	
	m_aData = new T[m_iCapacity];

	for (int i = 0; i < m_iCapacity; i++)
		m_aData[i] = item.m_aData[i];

	return *this;
}
//--------------------------------------------------------------------------------
template <class T>
void TArray<T>::add(const T& element)
{
	if ( m_iQuantity == m_iCapacity )
	{
		m_iCapacity = m_iCapacity * 2 + 1;

		T* aNewArray = new T[m_iCapacity];
		for ( int i = 0; i < m_iQuantity; i++ )
			aNewArray[i] = m_aData[i];

		delete[] m_aData;
		m_aData = aNewArray;
	}

	m_aData[m_iQuantity] = element;
	m_iQuantity++;
}
//--------------------------------------------------------------------------------
template <class T>
void TArray<T>::remove(int index)
{
	if ((index >= 0) && (index < m_iQuantity))
	{
		for ( int i = index; i < m_iQuantity-1; i++ )
		{
			m_aData[i] = m_aData[i+1];
		}
		m_iQuantity--;
	}
}
//--------------------------------------------------------------------------------
template <class T>
void TArray<T>::empty( )
{
	m_iQuantity = 0;
}
//--------------------------------------------------------------------------------
template <class T>
int TArray<T>::count() const
{
	return( m_iQuantity );
}
//--------------------------------------------------------------------------------
template <class T>
bool TArray<T>::contains( const T& element ) const
{
	for ( int i = 0; i < m_iQuantity; i++ )
	{
		if ( m_aData[i] == element )
			return true;
	}

	return false;
}
//--------------------------------------------------------------------------------
template <class T>
int TArray<T>::find( const T& element ) const
{
	for ( int i = 0; i < m_iQuantity; i++ )
	{
		if ( m_aData[i] == element )
			return( i );
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
template <class T>
bool TArray<T>::inrange( int index ) const
{
	return( ( index >= 0 ) && ( index < m_iQuantity ) );
}
//--------------------------------------------------------------------------------
template <class T>
T& TArray<T>::operator[] (int i)
{
	if ( i < 0 )
		i = 0;

	if ( i > m_iQuantity )
		i = m_iQuantity - 1;

	return m_aData[i];
}
//--------------------------------------------------------------------------------
template <class T>
const T& TArray<T>::operator[] (int i) const
{
	if ( i < 0 )
		i = 0;

	if ( i > m_iQuantity )
		i = m_iQuantity - 1;

	return m_aData[i];
}
//--------------------------------------------------------------------------------