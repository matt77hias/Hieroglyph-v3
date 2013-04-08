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
// AttributeEvaluator2f
//
// This attribute evaluator is an abstract base class that takes two floating 
// point inputs and must use the provided model, and fill in some portion of the
// passed in vertex.  It can do a single sub-element of the vertex, or more of it
// depending on what the designer intends.
//--------------------------------------------------------------------------------
#ifndef AttributeEvaluator2f_h
#define AttributeEvaluator2f_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class TModel, class TVertex>
	class AttributeEvaluator2f
	{
	public:
		virtual void evaluate( TModel& model, TVertex& v, float x, float y ) = 0;
	};

};
//--------------------------------------------------------------------------------
#endif // AttributeEvaluator2f_h
