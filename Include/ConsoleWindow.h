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
// WinConsole.h: interface for the ConsoleWindow class.
//
//--------------------------------------------------------------------------------
#ifndef ConsoleWindow_h
#define ConsoleWindow_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ScriptManager.h"

//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ConsoleWindow  
	{
	public:
		static HWND StartConsole(HINSTANCE hInstance, ScriptManager* pScriptContext);
		static void StopConsole();
		static void Write(const char *pString);

		static LRESULT WINAPI MsgProc( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool	ConsoleReady(void)	{return m_hWnd!= NULL;}

		static HWND GetHandle();

	private:
		ConsoleWindow();
		virtual ~ConsoleWindow();

		void	Init(HINSTANCE hInstance);
		void	ResizeControls(void);
		void	AdjustScrollBar(void);
		void	Paint(HDC hDC);


	private:
		static volatile bool	m_bWinIsActive;
		static volatile HWND m_hWnd;
		static volatile HWND m_hEditControl;
		static char m_CommandBuffer[4096];
		static wchar_t m_CommandBufferW[4096];
		
		std::list<std::string> m_stringList;
		HINSTANCE m_hInstance;

		ScriptManager* m_pScriptContext;

		int m_ScrollyPos;
		int m_textAreaHeight;
	};

	extern ConsoleWindow *g_Console;
};
//--------------------------------------------------------------------------------
#endif // ConsoleWindow_h
