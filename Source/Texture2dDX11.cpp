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
#include "Texture2dDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Texture2dDX11::Texture2dDX11( Microsoft::WRL::ComPtr<ID3D11Texture2D> pTex )
{
	m_pTexture = pTex;

	ZeroMemory( &m_DesiredDesc, sizeof( D3D11_TEXTURE2D_DESC ) );
	ZeroMemory( &m_ActualDesc, sizeof( D3D11_TEXTURE2D_DESC ) );
}
//--------------------------------------------------------------------------------
Texture2dDX11::~Texture2dDX11()
{
}
//--------------------------------------------------------------------------------
ResourceType Texture2dDX11::GetType()
{
	return( RT_TEXTURE2D );
}
//--------------------------------------------------------------------------------
D3D11_TEXTURE2D_DESC Texture2dDX11::GetActualDescription()
{
	ZeroMemory( &m_ActualDesc, sizeof( D3D11_TEXTURE2D_DESC ) );

	if ( m_pTexture )
		m_pTexture->GetDesc( &m_ActualDesc );

	return( m_ActualDesc );
}
//--------------------------------------------------------------------------------
D3D11_TEXTURE2D_DESC Texture2dDX11::GetDesiredDescription()
{
	return( m_DesiredDesc );
}
//--------------------------------------------------------------------------------
void Texture2dDX11::SetDesiredDescription( D3D11_TEXTURE2D_DESC description )
{
	m_DesiredDesc = description;
}
//--------------------------------------------------------------------------------
ID3D11Resource* Texture2dDX11::GetResource()
{
	return( m_pTexture.Get() );
}
//--------------------------------------------------------------------------------
UINT Texture2dDX11::GetEvictionPriority()
{
	UINT priority = 0;

	if ( m_pTexture )
		priority = m_pTexture->GetEvictionPriority();

	return( priority );
}
//--------------------------------------------------------------------------------
void Texture2dDX11::SetEvictionPriority( UINT EvictionPriority )
{
	if ( m_pTexture )
		m_pTexture->SetEvictionPriority( EvictionPriority );
}
//--------------------------------------------------------------------------------