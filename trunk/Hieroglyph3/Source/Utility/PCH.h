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
#include <math.h>
#include <memory.h>

// Boost includes
#include <boost/smart_ptr/shared_ptr.hpp>

// DirectX SDK includes
#include <dxgi.h>
#include <d3d11.h>
#include <d3dCompiler.h>
#include <d3dx11.h>
#include <D3DX10core.h>
#include <D3D11Shader.h>
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

#endif // PCH_h