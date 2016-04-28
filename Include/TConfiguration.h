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
// TConfiguration
//
// This simple template class allows for an encapsulated way to have a 
// configuration that can be set to a desired value, but that new value is only
// applied when it is explicitly 'applied' through a method call.  This allows
// consumers of a configuration to set it without knowledge of the inner workings
// of the configuration owner regarding update timing requirements.
//--------------------------------------------------------------------------------
#ifndef TConfiguration_h
#define TConfiguration_h
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class T>
	class TConfiguration
	{
	public:
		TConfiguration();
		virtual ~TConfiguration();

		void SetConfiguration( const T& config );
		void ApplyConfiguration( );
		const T& GetConfiguration( );

	private:
		T m_CurrentConfig;
		T m_NextConfig;
	};

#include "TConfiguration.inl"
};
//--------------------------------------------------------------------------------
#endif // TConfiguration_h
