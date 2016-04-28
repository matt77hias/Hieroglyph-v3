//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#pragma once
#include "NativeGlyph.h"
//--------------------------------------------------------------------------------
using namespace System;
using namespace Glyph3;
//--------------------------------------------------------------------------------
namespace GlyphCLR {

	public ref class ManagedGlyphlet
	{
	public:
		ManagedGlyphlet();
		~ManagedGlyphlet();

		void Initialize();
		IntPtr GetRenderTarget();
		void Update( float time );
		void Shutdown();

	private:
		NativeGlyph* m_pNative;
	};
}
//--------------------------------------------------------------------------------