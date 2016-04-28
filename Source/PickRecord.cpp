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
#include "PCH.h"
#include "PickRecord.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PickRecord::PickRecord( )
{
	pEntity = 0;
	fDistance = 0.0f;
}
//--------------------------------------------------------------------------------
PickRecord::~PickRecord()
{
}
//--------------------------------------------------------------------------------
