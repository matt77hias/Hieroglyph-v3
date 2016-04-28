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
// MaterialManager
//
//--------------------------------------------------------------------------------
#ifndef MaterialManager_h
#define MaterialManager_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "MaterialTemplate.h"
#include "MaterialDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MaterialManager
	{
	public:
		MaterialManager();
		virtual ~MaterialManager();

		void AddTemplate( MaterialTemplate* pTemplate );

		//MaterialPtr ProduceMaterial( std::wstring name, bool shared = true );

	protected:
		std::map< std::wstring, MaterialTemplate* > m_Templates;
	};
};
//--------------------------------------------------------------------------------
#endif // MaterialManager_h
//--------------------------------------------------------------------------------

