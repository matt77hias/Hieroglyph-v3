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
#include "RiftHMD.h"
#include <stdexcept>
#include "Kernel/OVR_Math.h"
#include "OVR_Stereo.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RiftHMD::RiftHMD( RiftManagerPtr RiftMgr ) : 
	m_RiftMgr( RiftMgr ),
	m_hmd( nullptr )
{
	// Create the first device available.  If it isn't available then we throw an
	// exception to let the user know.

	m_hmd = ovrHmd_Create(0);
	if ( !m_hmd ) throw std::invalid_argument( "No HMD Devices were found!" );
	
	// Start the sensor which provides the Rift’s pose and motion.
	ovrHmd_StartSensor( m_hmd, ovrSensorCap_Orientation |
                            ovrSensorCap_YawCorrection |
                            ovrSensorCap_Position, 0 );

	ovrHmdDesc HmdDesc = GetHMDDesc();
	
    // Initialize FovSideTanMax, which allows us to change all Fov sides at once - Fov
    // starts at default and is clamped to this value.
    float FovSideTanLimit = OVR::FovPort::Max(HmdDesc.MaxEyeFov[0], HmdDesc.MaxEyeFov[1]).GetMaxSideTan();
    float FovSideTanMax   = OVR::FovPort::Max(HmdDesc.DefaultEyeFov[0], HmdDesc.DefaultEyeFov[1]).GetMaxSideTan();

    // Clamp Fov based on our dynamically adjustable FovSideTanMax.
    // Most apps should use the default, but reducing Fov does reduce rendering cost.
    eyeFov[0] = OVR::FovPort::Min( HmdDesc.DefaultEyeFov[0], OVR::FovPort(FovSideTanMax) );
    eyeFov[1] = OVR::FovPort::Min( HmdDesc.DefaultEyeFov[1], OVR::FovPort(FovSideTanMax) );

}
//--------------------------------------------------------------------------------
RiftHMD::~RiftHMD()
{
	if ( m_hmd ) {
		ovrHmd_Destroy( m_hmd );
		m_hmd = nullptr;
	}
}
//--------------------------------------------------------------------------------
Matrix3f RiftHMD::GetOrientation( double time )
{
	// Query the HMD for the sensor state at a given time. "0.0" means "most recent time".
	ovrSensorState ss = ovrHmd_GetSensorState( m_hmd, 0.0 );

	Matrix3f orientation;
	orientation.MakeIdentity();

	if ( ss.StatusFlags & ( ovrStatus_OrientationTracked | ovrStatus_PositionTracked ) )
	{
		float yaw, eyePitch, eyeRoll = 0.0f;
		
		// Get the most recent pose information from the sensor state
		OVR::Transformf movePose = ss.Recorded.Pose; //ss.Predicted.Pose;
		
		// Get the Euler angles from teh quaternion
		movePose.Rotation.GetEulerAngles< OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z, OVR::Rotate_CCW, OVR::Handed_L >( &yaw, &eyePitch, &eyeRoll );
	
		// Apply those angles to our rotation matrix
		orientation.Rotation( Vector3f( eyePitch, yaw, -eyeRoll ) );
	}

	return( orientation );
}
//--------------------------------------------------------------------------------
ovrHmd RiftHMD::GetHMD()
{
	return( m_hmd );
}
//--------------------------------------------------------------------------------
ovrHmdDesc RiftHMD::GetHMDDesc()
{
	ovrHmdDesc hmdDesc;
	ovrHmd_GetDesc( m_hmd, &hmdDesc );

	return( hmdDesc );
}
//--------------------------------------------------------------------------------
Matrix4f RiftHMD::GetPerspectiveFov( unsigned int eye, float zn, float zf  )
{
	// This method is based on the perspective matrix in the Oculus SDK.  It is 
	// reimplemented here to allow direct initialization of the Hieroglyph 3 
	// specific matrix object.

	Matrix4f p;

	float xScale = 2.0f / ( eyeFov[eye].LeftTan + eyeFov[eye].RightTan );
	float xOffset = ( eyeFov[eye].LeftTan - eyeFov[eye].RightTan ) * xScale * 0.5f;

	float yScale = 2.0f / ( eyeFov[eye].UpTan + eyeFov[eye].DownTan );
	float yOffset = ( eyeFov[eye].UpTan - eyeFov[eye].DownTan ) * yScale * 0.5f;

	// row 0
	p[ 0] = xScale;
	p[ 1] = 0.0f;
	p[ 2] = 0.0f;
	p[ 3] = 0.0f;

	// row 1
	p[ 4] = 0.0f;
	p[ 5] = yScale;
	p[ 6] = 0.0f;
	p[ 7] = 0.0f;

	// row 2
	p[ 8] = xOffset;
	p[ 9] = -yOffset;
	p[10] = zf / ( zf-zn );
	p[11] = 1.0f;

	// row 3
	p[12] = 0.0f;
	p[13] = 0.0f;
	p[14] = -zn*zf / ( zf-zn );
	p[15] = 0.0f;

	return p;
}
//--------------------------------------------------------------------------------