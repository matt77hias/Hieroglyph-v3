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
//--------------------------------------------------------------------------------
#ifndef RiftHMD_h
#define RiftHMD_h
//--------------------------------------------------------------------------------
#include "RiftManager.h"
#include "OVR_CAPI.h"
#include "Matrix3f.h"
#include "Texture2dConfigDX11.h"
//--------------------------------------------------------------------------------
// Configure D3D11 as the Rendering API used, which is then also used for API
// specific distortion rendering.
#define OVR_D3D_VERSION 11
#include "OVR_CAPI_D3D.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class RiftHMD
	{
	public:
		RiftHMD( RiftManagerPtr RiftMgr );
		~RiftHMD();

		ovrHmdDesc GetHMDDesc();

		Matrix3f GetOrientation( double time );
		Matrix4f GetPerspectiveFov( unsigned int eye, float zn, float zf );

		ovrHmd GetHMD();

		ovrFovPort eyeFov[2];
		ovrEyeRenderDesc eyeRenderDesc[2];
		ovrD3D11Texture eyeTexture[2];
		ovrRecti eyeViewports[2];

	private:
		ovrHmd m_hmd;
		RiftManagerPtr m_RiftMgr;
	};

	typedef std::shared_ptr<RiftHMD> RiftHMDPtr;
};
//--------------------------------------------------------------------------------
#endif // RiftHMD_h
//--------------------------------------------------------------------------------