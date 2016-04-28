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
#include "EventManager.h"
#include "ConsoleActor.h"
#include "EvtChar.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ConsoleActor::ConsoleActor() : 
	console(),
	textActor()
{
	RequestEvent( SYSTEM_KEYBOARD_CHAR );

	GetNode()->AttachChild( textActor.GetNode() );

	textActor.SetTextOrientation( Vector3f( 1.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
	textActor.GetNode()->Transform.Position() = Vector3f( -0.5f, 0.2f, 1.0f );
	textActor.SetCharacterHeight( 0.02f );

	// Modify the text actor's material with a view space version of its shaders.
	textActor.GetBody()->Visual.GetMaterial()->Params[VT_PERSPECTIVE].pEffect->SetVertexShader( 
		RendererDX11::Get()->LoadShader( VERTEX_SHADER,
		std::wstring( L"AlphaTestTexturedVS.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_4_0" ) ) );

	textActor.GetBody()->Visual.GetMaterial()->Params[VT_PERSPECTIVE].pEffect->SetPixelShader( 
		RendererDX11::Get()->LoadShader( PIXEL_SHADER,
		std::wstring( L"AlphaTestTexturedVS.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_4_0" ) ) );

	printText();
}
//--------------------------------------------------------------------------------
ConsoleActor::~ConsoleActor()
{
}
//--------------------------------------------------------------------------------
bool ConsoleActor::HandleEvent( EventPtr pEvent )
{
    eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_CHAR )
    {
        EvtCharPtr pChar = std::static_pointer_cast<EvtChar>(pEvent);
		UINT key = pChar->GetCharacterCode();

		console.processKey( key );
		printText();
	}

    return false;
}
//--------------------------------------------------------------------------------
std::wstring Glyph3::ConsoleActor::GetName()
{
    return L"First Person Camera";
}
//--------------------------------------------------------------------------------
void ConsoleActor::printText()
{
	// Draw the characters for the visible portion of the console window.
	textActor.SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	textActor.SetText( L">> " + GlyphString::ToUnicode(console.line) );
	textActor.NewLine();

	textActor.SetColor( Vector4f( 0.5f, 0.5f, 1.0f, 1.0f ) );

	size_t count = 0;
	for ( auto entry = console.history.rbegin(); entry != console.history.rend(); ++entry ) {
		textActor.AppendText( GlyphString::ToUnicode( *entry ) );
		textActor.NewLine();
		++count;
		if ( count >= MAX_ENTRY_DISPLAY ) break;
	}
}
//--------------------------------------------------------------------------------