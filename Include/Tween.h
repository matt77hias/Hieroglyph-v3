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
// Tween
//
//--------------------------------------------------------------------------------
#ifndef Tween_h
#define Tween_h
//--------------------------------------------------------------------------------
#include "IController.h"
#include <functional>
#include "Entity3D.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <typename TValue>
	TValue Linear( const TValue& start, const TValue& end, float t );

	template <typename TValue>
	TValue QuadraticInOut( const TValue& start, const TValue& end, float t );

	template <typename TValue>
	TValue Hermite( const TValue& start, const TValue& end, float t );

	#include "Tween.inl"
};
//--------------------------------------------------------------------------------
#endif // Tween_h
//--------------------------------------------------------------------------------