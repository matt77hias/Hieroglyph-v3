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
// TStateArrayMonitor
//
//--------------------------------------------------------------------------------
#ifndef TStateArrayMonitor_h
#define TStateArrayMonitor_h
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class T, unsigned int N>
	class TStateArrayMonitor
	{
	public:
		TStateArrayMonitor( T initialState );
		~TStateArrayMonitor();

		void SetSister( TStateArrayMonitor<T,N>* pSister );
		bool SameAsSister( unsigned int slot );
		
		void SetState( unsigned int slot, T state );
	
		bool IsUpdateNeeded();
		unsigned int GetStartSlot();
		unsigned int GetEndSlot();
		unsigned int GetRange();

		T GetState( unsigned int slot ) const;
		T* GetFirstSlotLocation();
		T* GetSlotLocation( unsigned int slot );

		void InitializeStates();
		void ResetTracking();

	private:

		void SearchFromBelow();
		void SearchFromAbove();

		// The monitoring varaibles
		unsigned int m_uiStartSlot;
		unsigned int m_uiEndSlot;
		bool m_bUploadNeeded;

		// The state data
		T m_InitialState;
		T m_States[N];

		// The sister state
		TStateArrayMonitor<T,N>* m_pSister;
	};

#include "TStateArrayMonitor.inl"
};
//--------------------------------------------------------------------------------
#endif // TStateArrayMonitor_h
