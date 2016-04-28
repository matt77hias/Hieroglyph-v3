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
// RenderParameterDX11
//
//--------------------------------------------------------------------------------
#ifndef RenderParameterDX11_h
#define RenderParameterDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	enum ParameterType
	{
		VECTOR,
		MATRIX,
		MATRIX_ARRAY,
		SHADER_RESOURCE,
		UNORDERED_ACCESS,
		CBUFFER,
		SAMPLER,
		ENTITY
	};

	class RenderParameterDX11
	{
	public:
		RenderParameterDX11();
		RenderParameterDX11( RenderParameterDX11& copy );
		virtual ~RenderParameterDX11();

		void SetName( const std::wstring& name );
		std::wstring& GetName();

		// Each parameter type will implement this method for a simple way
		// to tell what kind of data it uses.  This is important for handling
		// the parameters in a generic way, but still being able to perform
		// type checking.

		virtual const ParameterType GetParameterType() = 0;

		// Initializing parameter data sets all copies of this resource with
		// the provided data.  Internally this calls the SetParameterData() 
		// method on each thread's data copy, which is specialized by each 
		// concrete subclass.  The uninitialize method will reset the parameter
		// to a default value if the parameter's data matches the passed in data.

		void InitializeParameterData( void* pData );
		//void UnInitializeParameterData( void* pData );

		// Setting parameter data does what it sounds like - it takes a pointer
		// to the data and sets it in the data copy indicated by its thread ID.
		// Reset will default the data if the current data matches the passed in
		// data.

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 ) = 0;
		//virtual void ResetParameterData( void* pData, unsigned int threadID = 0 ) = 0;

		// This update function is a convenience function to allow 
		// RenderParameterDX11 subclasses to be referred to by their parent class
		// and automatically update the parameter stored in the parameter
		// manager.
		
		//virtual void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 ) = 0;

		// The value ID is used to efficiently determine when a parameter's value
		// has been changed.  Each parameter type will increment the value ID
		// after it has been changed to indicate that it should be re-evaluated.
		// This is especially important for updating constant buffer contents
		// only when needed.

		unsigned int GetValueID( unsigned int threadID = 0 );

	protected:
		std::wstring	m_sParameterName;
		unsigned int	m_auiValueID[NUM_THREADS+1];
	};
};
//--------------------------------------------------------------------------------
#endif // RenderParameterDX11_h
//--------------------------------------------------------------------------------

