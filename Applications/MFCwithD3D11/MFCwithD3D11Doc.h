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
// CMFCwithD3D11Doc
//
// This document subclass is used to provide a renderable scene for use in this 
// example program.  This is currently a hard coded scene, but could also be 
// loaded from file or whatever other mechanism would be needed to generate a 
// renderable scene.
//--------------------------------------------------------------------------------

#pragma once

//--------------------------------------------------------------------------------
class CMFCwithD3D11Doc : public CDocument
{

protected: // create from serialization only
	CMFCwithD3D11Doc();
	DECLARE_DYNCREATE(CMFCwithD3D11Doc)

// Attributes
public:
	
	// In this document, our data is represented by a scene object.
	// Upon creation, the scene is populated with some example content,
	// and is then referenced through the scene object only.

	Glyph3::Scene*	pScene;
	Glyph3::Actor*	pActor;

	static CMFCwithD3D11Doc* GetDoc();

// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CMFCwithD3D11Doc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DeleteContents();
};
//--------------------------------------------------------------------------------