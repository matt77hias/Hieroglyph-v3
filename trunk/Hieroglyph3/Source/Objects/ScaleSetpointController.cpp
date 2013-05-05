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
#include "ScaleSetpointController.h"
#include "Entity3D.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ScaleSetpointController::ScaleSetpointController() :
	m_vStartpoint( 1.0f, 1.0f, 1.0f ),
	m_vSetpoint( 1.0f, 1.0f, 1.0f ),
	m_fDuration( 0.0f ),
	m_fElapsed( 0.0f ),
	m_bActive( false )
{
}
//--------------------------------------------------------------------------------
ScaleSetpointController::~ScaleSetpointController()
{
}
//--------------------------------------------------------------------------------
void ScaleSetpointController::Update( float fTime )
{
	if ( m_bActive )
	{
		// Calculate the scale to apply to the entity.
		m_fElapsed += fTime;

		if ( m_fElapsed >= m_fDuration )
		{
			m_fElapsed = m_fDuration;
			m_bActive = false;
		}

		float t = m_fElapsed / m_fDuration;
		t = t*t*t;
	
		Vector3f vInterpolant = m_vStartpoint*(1.0f-t) + m_vSetpoint*t;

		// Set the scale in the entity.
		if ( m_pEntity )
		{
			m_pEntity->Scale( ) = vInterpolant;
		}
	}
}
//--------------------------------------------------------------------------------
void ScaleSetpointController::SetSetpoint( const Vector3f& scale, float duration )
{
	m_vSetpoint = scale;
	m_vStartpoint = m_pEntity->Scale();

	m_fDuration = duration;
	m_fElapsed = 0.0f;

	m_bActive = true;
}
//--------------------------------------------------------------------------------