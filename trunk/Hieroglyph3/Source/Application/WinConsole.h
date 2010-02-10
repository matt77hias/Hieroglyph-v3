//--------------------------------------------------------------------------------
// WinConsole.h: interface for the CWinConsole class.
//
//--------------------------------------------------------------------------------
#ifndef CWinConsole_h
#define CWinConsole_h
//--------------------------------------------------------------------------------
#include "windows.h"
#include "CScriptManager.h"
#include <string>
#include <list>
//--------------------------------------------------------------------------------
namespace Glyph2
{
	class CWinConsole  
	{
	public:
		static HWND StartConsole(HINSTANCE hInstance, CScriptManager* pScriptContext);
		static void StopConsole();
		static void Write(const char *pString);

		static LRESULT WINAPI MsgProc( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK SubclassInputEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool	ConsoleReady(void)	{return m_hWnd!= NULL;}

		static HWND GetHandle();

	private:
		CWinConsole();
		virtual ~CWinConsole();

		void	Init(HINSTANCE hInstance);
		void	ResizeControls(void);
		void	AdjustScrollBar(void);
		void	Paint(HDC hDC);


	private:
		static volatile bool	m_bWinIsActive;
		static volatile HWND m_hWnd;
		static volatile HWND m_hEditControl;
		static char m_CommandBuffer[4096];
		
		std::list<std::string> m_stringList;
		HINSTANCE m_hInstance;

		CScriptManager* m_pScriptContext;

		int m_ScrollyPos;
		int m_textAreaHeight;
	};

	extern CWinConsole *g_Console;
};
//--------------------------------------------------------------------------------
#endif // CWinConsole_h
