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
// RiftHMD
//
// This class manages the Oculus Rift device handle, and provides some helper
// functions for getting access to some of the data we need from the device.  
//
// The class implements the Pimpl idiom in order to hide all of the types defined 
// by the Oculus SDK.  This allows us to statically link our implementation to the
// Oculus static library, and means the users of the Hieroglyph 3 SDK don't need
// access to the Oculus SDK.  This is very useful when you have lots of 
// applications that will use this feature, because then you have one less 
// dependency.
//--------------------------------------------------------------------------------
#ifndef RiftHMD_h
#define RiftHMD_h
//--------------------------------------------------------------------------------
#include "RiftManager.h"
#include "Matrix3f.h"
#include "Texture2dConfigDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class RiftHMD
	{
	public:
		RiftHMD( RiftManagerPtr RiftMgr );
		~RiftHMD();

		unsigned int HmdDisplayWidth();
		unsigned int HmdDisplayHeight();
		unsigned int DesiredEyeTextureWidth();
		unsigned int DesiredEyeTextureHeight();

		void ConfigureRendering( const ResourcePtr& renderTarget, int swapchain );
		void ConfigureEyeTexture( unsigned int eye, const ResourcePtr& texture );

		Matrix3f GetOrientation( double time );
		Matrix4f GetPerspectiveFov( unsigned int eye, float zn, float zf );
		Matrix4f GetEyeTranslation( unsigned int eye );

		float BeginFrame();
		void BeginEyeRender( unsigned int eye );
		void EndEyeRender( unsigned int eye );
		void EndFrame();

	private:
		struct Impl;
		Impl* m_pImpl;
	};

	typedef std::shared_ptr<RiftHMD> RiftHMDPtr;
};
//--------------------------------------------------------------------------------
#endif // RiftHMD_h
//--------------------------------------------------------------------------------