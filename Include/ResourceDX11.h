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
// ResourceDX11
//
// This pure interface provides the function interface to the information 
// contained in the D3D11 resource class.  The functions are made abstract so that
// the concrete classes that implement this interface will provide their own
// implementations based on their individual types, instead of providing a 
// resource pointer here and forcing each subclass to cast its type.
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
#ifndef ResourceDX11_h
#define ResourceDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ResourceDX11
	{
	public:
		ResourceDX11();

		virtual ~ResourceDX11();

		virtual ResourceType				GetType() = 0;
		virtual ID3D11Resource*				GetResource() = 0;

		virtual UINT						GetEvictionPriority() = 0;
		virtual void						SetEvictionPriority( UINT EvictionPriority ) = 0;

		unsigned short						GetInnerID();

		static unsigned short				s_usResourceUID;
		unsigned short						m_usInnerID;
	};

};
//--------------------------------------------------------------------------------
#endif // ResourceDX11_h
//--------------------------------------------------------------------------------

