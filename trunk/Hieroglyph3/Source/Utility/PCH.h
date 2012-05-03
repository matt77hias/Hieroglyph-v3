//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// PCH.h
//
// Used for generated a precompiled header file
//--------------------------------------------------------------------------------
#ifndef PCH_h
#define PCH_h
//--------------------------------------------------------------------------------

#ifndef STRICT
#define STRICT
#endif

// Standard C++ Library includes
#include <fstream>
#include <string>
#include <map>
#include <list>
#include <sstream>
#include <algorithm>
#include <map>

// Standard C Library includes
#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
#include <math.h>
#include <memory.h>
#include <assert.h>

// DirectX SDK includes
#include <D3Dcommon.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dCompiler.h>
#include <d3dx11.h>
//#include <D3DX10core.h>
//#include <D3D11Shader.h>

#include <d3d9.h>

// Windows SDK includes
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <initguid.h>
#include <shlobj.h>

// Library imports
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3dx10.lib" )
#pragma comment( lib, "DXGI.lib" )
#pragma comment( lib, "d3d9.lib" )

#define SAFE_RELEASE( x ) {if(x){(x)->Release();(x)=NULL;}}
#define SAFE_DELETE( x ) {if(x){delete (x);(x)=NULL;}}
#define SAFE_DELETE_ARRAY( x ) {if(x){delete[] (x);(x)=NULL;}}

// Define the number of threads to use for 
#define NUM_THREADS 4

#define GLYPH_PI 3.14159265


#endif // PCH_h