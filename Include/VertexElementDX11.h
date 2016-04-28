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
// VertexElementDX11
//
// This is a class to represent generic vertex information.  The elements
// themselves can be either 1 thru 4 floating point values per element.
//
// Currently the arrays are a fixed size.  This may change in the future, but for
// now the user must create new CVertexElements with the new size and manually
// copy over the vertex data to the new object.
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef VertexElementDX11_h
#define VertexElementDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VertexElementDX11
	{

    public:

        // Standard semantic names
        static std::string PositionSemantic;
        static std::string NormalSemantic;
        static std::string TexCoordSemantic;
        static std::string BoneIDSemantic;
		static std::string BoneWeightSemantic;
        static std::string TangentSemantic;

	public:
		VertexElementDX11( int tuple, int elementCount );
		~VertexElementDX11( );
		
		int				SizeInBytes();
		int				Count();
		int				Tuple();

		void*			GetPtr( int i );

		float*			Get1f( int i );
		Vector2f*		Get2f( int i );
		Vector3f*		Get3f( int i );
		Vector4f*		Get4f( int i );

		int*			Get1i( int i );

		unsigned int*	Get1ui( int i );

		float*					operator[]( int i );
		const float*			operator[]( int i ) const;

		std::string						m_SemanticName;
		UINT							m_uiSemanticIndex;
		DXGI_FORMAT						m_Format;
		UINT							m_uiInputSlot;
		UINT							m_uiAlignedByteOffset;
		D3D11_INPUT_CLASSIFICATION		m_InputSlotClass;
		UINT							m_uiInstanceDataStepRate;

	protected:
		VertexElementDX11();

		float*							m_pfData;
		int								m_iTuple;
		int								m_iCount;
	};
};
#endif // VertexElementDX11_h
