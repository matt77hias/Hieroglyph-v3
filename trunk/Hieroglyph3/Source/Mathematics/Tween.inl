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
template <class TValue>
TValue Tween<TValue>::Linear( TValue start, TValue end, float t )
{
	return( start*(1.0f-t) + end*(t) );
}
//--------------------------------------------------------------------------------
template <class TValue>
TValue Tween<TValue>::QuadraticInOut( TValue start, TValue end, float t )
{
	float s;

	if ( t < 0.5f ) {
		t *= 2.0f;
		s = t*t*0.5f;
	} else {
		t = (t-1.0f)*2.0f;
		s = (1.0f-(t*t)) * 0.5f + 0.5f;
	}
	return( start*(1.0f-s) + end*(s) );
}
//--------------------------------------------------------------------------------
