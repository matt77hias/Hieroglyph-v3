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
// GeometryLoaderDX11
//
//--------------------------------------------------------------------------------
#ifndef GeometryLoaderDX11_h
#define GeometryLoaderDX11_h
//--------------------------------------------------------------------------------
#include "GeometryDX11.h"
#include "SkinnedActor.h"
#include <string>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class GeometryLoaderDX11
	{
	public:
		//static GeometryDX11* loadPlyFile( std::wstring filename );
		//static GeometryDX11* loadMS3DFile( std::wstring filename );		
		static GeometryPtr loadMS3DFile2( std::wstring filename );		
		static GeometryPtr loadMS3DFileWithAnimationAndWeights( std::wstring filename, SkinnedActor* pActor = 0 );
		static GeometryPtr loadMS3DFileWithAnimation( std::wstring filename, SkinnedActor* pActor = 0 );	
		
		
		//static void removeWhiteSpace( std::wstring& s );
		//static std::wstring getElementName( int usage, int index );

		static GeometryPtr loadStanfordPlyFile( std::wstring filename, bool withAdjacency = false );

	private:
		GeometryLoaderDX11();

		struct PlyElementPropertyDeclaration
		{
			std::string name;
			bool isList;
			std::string type;
			std::string listLengthType;
		};

		struct PlyElementDesc
		{
			std::string name;
			int elementCount;
			std::vector< PlyElementPropertyDeclaration > dataFormat;
			std::vector< void** > data;
		};

		template<typename T>
		struct PlyDataArray
		{
			unsigned int length;
			T* data;
		};

		static int FindAdjacentIndex( int edgeStart, int edgeEnd, int triV, int* pRaw, int rawLen);
		static PlyElementDesc ParsePLYElementHeader(std::string headerLine, std::ifstream& input);
		static PlyElementPropertyDeclaration ParsePLYElementProperty(std::string desc);
		static PlyElementPropertyDeclaration ParsePLYElementPropertyList(std::string desc);
		static std::vector<void**> ReadPLYElementData(std::ifstream& input, const PlyElementDesc& desc);
		static void** ParsePLYElementData(std::string text, const std::vector<PlyElementPropertyDeclaration>& desc);
		template<typename T> static T* ExtractDataPtr(std::string input);
		template<typename T> static T ExtractDataVal(std::string input);
		template<typename T> static PlyDataArray<T>* ExtractDataPtrArray(int length, std::vector<std::string>::iterator iterator);
		static int FindPlyElementIndex(std::vector<PlyElementDesc> elems, std::string name);
		static int FindPlyElementPropertyIndex(std::vector<PlyElementPropertyDeclaration> elems, std::string name);
	};
};
#endif // GeometryLoaderDX11_h


