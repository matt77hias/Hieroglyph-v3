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
// SpatialController
//
//--------------------------------------------------------------------------------
#ifndef SpatialController_h
#define SpatialController_h
//--------------------------------------------------------------------------------
#include "IController.h"
#include "Vector3f.h"
#include "Entity3D.h"
#include "Log.h"
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <typename T>
	class SpatialController : public IController<T>
	{
	public:
		SpatialController( );
		SpatialController( const Vector3f& translation, const Vector3f& rotation );
		virtual ~SpatialController( );

		virtual void Update( float fTime );

		void SetRotation( const Vector3f& xyz );
		void SetTranslation( const Vector3f& translation );

		Vector3f GetRotation();
		Vector3f GetTranslation();

		void RotateBy( const Vector3f& xyz );
		void RotateXBy( const float x );
		void RotateYBy( const float y );
		void RotateZBy( const float z );

		void TranslateBy( Vector3f& xyz );
		void TranslateXBy( const float x );
		void TranslateYBy( const float y );
		void TranslateZBy( const float z );

		void MoveForward( const float distance );
		void MoveBackward( const float distance );
		void MoveRight( const float distance );
		void MoveLeft( const float distance );
		void MoveUp( const float distance );
		void MoveDown( const float distance );

	protected:
		Vector3f		m_vRotation;
		Vector3f		m_vTranslation;
	};

	#include "SpatialController.inl"
};
//--------------------------------------------------------------------------------
#endif // SpatialController_h
