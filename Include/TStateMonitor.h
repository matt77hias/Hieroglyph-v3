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
// TStateMonitor
//
//--------------------------------------------------------------------------------
#ifndef TStateMonitor_h
#define TStateMonitor_h
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class T>
	class TStateMonitor
	{
	public:
		TStateMonitor( T initialState );
		~TStateMonitor();

		void SetSister( TStateMonitor<T>* pSister );
		bool SameAsSister();

		void SetState( T state );
		T GetState() const;

		bool IsUpdateNeeded();
		void InitializeState();
		void ResetTracking();

	private:

		// The monitoring varaibles
		bool m_bUploadNeeded;

		// The state data
		T m_InitialState;
		T m_State;

		// The sister state
		TStateMonitor<T>* m_pSister;
	};

#include "TStateMonitor.inl"
};
//--------------------------------------------------------------------------------
#endif // TStateMonitor_h
