//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "GlyphCLR.h"
#include "NativeGlyph.h"
//--------------------------------------------------------------------------------
using namespace GlyphCLR;
//--------------------------------------------------------------------------------
ManagedGlyphlet::ManagedGlyphlet() : m_pNative( nullptr )
{
	m_pNative = new NativeGlyph();
}
//--------------------------------------------------------------------------------
ManagedGlyphlet::~ManagedGlyphlet()
{
	delete m_pNative;
}
//--------------------------------------------------------------------------------
void ManagedGlyphlet::Initialize()
{
	m_pNative->Initialize();
}
//--------------------------------------------------------------------------------
IntPtr ManagedGlyphlet::GetRenderTarget()
{
	return( IntPtr( m_pNative->GetRenderTarget() ) );
}
//--------------------------------------------------------------------------------
void ManagedGlyphlet::Update( float time )
{
	m_pNative->Update( time );
}
//--------------------------------------------------------------------------------
void ManagedGlyphlet::Shutdown()
{
	m_pNative->Shutdown();
}
//--------------------------------------------------------------------------------