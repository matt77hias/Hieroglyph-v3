//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ConsoleWindow.h"
#include "ScriptManager.h"
#include "GlyphString.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
luaL_reg DebugGlue[];
ConsoleWindow* Glyph3::g_Console = NULL;

volatile bool ConsoleWindow::m_bWinIsActive = true;
volatile HWND ConsoleWindow::m_hWnd = NULL;
volatile HWND ConsoleWindow::m_hEditControl = NULL;

//char ConsoleWindow::m_CommandBuffer[4096];
wchar_t ConsoleWindow::m_CommandBufferW[4096];

WNDPROC lpfnInputEdit;  // Storage for subclassed edit control 
//--------------------------------------------------------------------------------
HWND ConsoleWindow::StartConsole( HINSTANCE hInstance, ScriptManager* pScriptContext )
{
	if(!g_Console)
	{
		g_Console = new ConsoleWindow();
	}

	if(!m_hWnd)
		g_Console->Init(hInstance);

	g_Console->m_pScriptContext = pScriptContext;
	
	// init the glue functions required for the debug window
	for(int i=0; DebugGlue[i].name; i++)
	{
		lua_register(pScriptContext->GetState(), DebugGlue[i].name, DebugGlue[i].func);
	}

	return (m_hWnd);
}
//--------------------------------------------------------------------------------
void ConsoleWindow::StopConsole()
{
	if( g_Console )
	{
		// The destructor of the console object will destroy the console window,
		// and reset the handles for the next creation of the window.

		delete g_Console;
		g_Console = NULL;
	}
}
//--------------------------------------------------------------------------------
ConsoleWindow::ConsoleWindow()
{
	m_hWnd = NULL;
	//memset( m_CommandBuffer, 0, 4096*sizeof(char) );
	memset( m_CommandBufferW, 0, 4096*sizeof(wchar_t) );
}
//--------------------------------------------------------------------------------
ConsoleWindow::~ConsoleWindow()
{
	if( m_hWnd )
	{
		DestroyWindow( m_hWnd );
		m_hWnd = 0;
	}
}
//--------------------------------------------------------------------------------
void ConsoleWindow::AdjustScrollBar(void)
{
	SCROLLINFO si;

	si.cbSize = sizeof(si); 
	si.fMask  = SIF_RANGE | SIF_PAGE | SIF_POS; 
	si.nMin   = 0; 
	si.nMax   = m_stringList.size(); 
	si.nPage  = m_textAreaHeight; 
	si.nPos   = m_stringList.size() - m_ScrollyPos; 
	SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE); 
}
//--------------------------------------------------------------------------------
void ConsoleWindow::ResizeControls(void)
{
	RECT r;

	GetClientRect(m_hWnd, &r);
	m_textAreaHeight = (r.bottom - r.top) / 16;

	SetWindowPos(m_hEditControl, HWND_TOP, r.left + 2, r.bottom - 18, r.right - r.left - 4, 16, SWP_NOZORDER);

	AdjustScrollBar();
	InvalidateRect(m_hWnd, NULL, TRUE);
}
//--------------------------------------------------------------------------------
void ConsoleWindow::Paint(HDC hDC)
{
	SetTextColor(hDC, RGB(64,255,64));
	SetBkColor(hDC, RGB(0,0,0));

	RECT r;

	GetClientRect(m_hWnd, &r);

	int x = 2;
	int y = r.bottom - 40;

	std::list<std::string>::iterator it = m_stringList.begin();
	int skip = m_ScrollyPos;
	while(skip)
	{
		++it;
		--skip;
	}

	while(it != m_stringList.end())
	{
		TextOut(hDC, x, y, GlyphString::ToUnicode(std::string(*it)).c_str(), strlen((*it).c_str()));
		y -= 16;
		++it;
	}
}
//--------------------------------------------------------------------------------
LRESULT WINAPI ConsoleWindow::MsgProc( HWND hWnd, unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
    {
		case WM_ACTIVATEAPP:
			m_bWinIsActive = (wParam != 0);
			return 0L;

		case WM_ACTIVATE:
			// Pause if minimized or not the top window
			m_bWinIsActive = (wParam == WA_ACTIVE) || (wParam == WA_CLICKACTIVE);
			return 0L;

		case WM_DESTROY:
			m_bWinIsActive = false;
			m_hWnd = NULL;
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			g_Console->Paint(hdc);
			EndPaint(hWnd, &ps);
			break;

		case WM_CHAR:
			break;

		case WM_LBUTTONDOWN:
			break;

		case WM_RBUTTONDOWN:
			break;

		case WM_LBUTTONUP:
			break;

		case WM_RBUTTONUP:
			break;

		case WM_MOUSEMOVE:
			break;

		case WM_COMMAND:
			break;

		case WM_SIZING:
		case WM_SIZE:
			g_Console->ResizeControls();
			break;

		case WM_SETCURSOR:
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			ShowCursor(TRUE);
			break;

    case WM_VSCROLL: 
        switch(wParam & 0xFFFF)//LOWORD (wParam)) 
        { 
            // User clicked the shaft above the scroll box. 
 
            case SB_PAGEUP: 
				//g_Console->m_ScrollyPos = std::min((int) (g_Console->m_ScrollyPos+g_Console->m_textAreaHeight), (int) (g_Console->m_stringList.size() - g_Console->m_textAreaHeight)+1);
                 //yInc = min(-1, -yClient / yChar); 
                 break; 
 
            // User clicked the shaft below the scroll box. 
 
            case SB_PAGEDOWN: 
                // yInc = max(1, yClient / yChar); 
                 break; 
 
            // User clicked the top arrow. 
 
            case SB_LINEUP: 
				//g_Console->m_ScrollyPos = std::min(g_Console->m_ScrollyPos+1, (int) (g_Console->m_stringList.size() - g_Console->m_textAreaHeight)+1);
                break; 
 
            // User clicked the bottom arrow. 
 
            case SB_LINEDOWN: 
                 //yInc = 1; 
				//g_Console->m_ScrollyPos = std::max(g_Console->m_ScrollyPos-1, 0);
                 break; 
 
            // User dragged the scroll box. 
 
            case SB_THUMBTRACK: 
                 //yInc = HIWORD(wParam) - yPos; 
                 break; 
 
            default: 
                 //yInc = 0;
				break;
 
        } 
		{
			SCROLLINFO si;
			si.cbSize = sizeof(si); 
            si.fMask  = SIF_POS; 
			si.nPos   = g_Console->m_stringList.size() - g_Console->m_ScrollyPos; 
            SetScrollInfo(hWnd, SB_VERT, &si, TRUE); 
		}
		InvalidateRect(m_hWnd, NULL, TRUE);

		break;

		case WM_USER:
			// command ready from edit control
			// string should be in m_CommandBuffer
			std::string command = GlyphString::ToAscii( std::wstring( m_CommandBufferW ) );
			Write(command.c_str());
			if (0 != luaL_loadbuffer( ScriptManager::Get()->GetState(), /* g_Console->m_pScriptContext->GetScriptContext()*/
					command.c_str(), command.length(), NULL))
			{
				Write("Error loading Command\n");
			}
			if (0 != lua_pcall( ScriptManager::Get()->GetState(), /*g_Console->m_pScriptContext->GetScriptContext()*/ 0, LUA_MULTRET, 0))
			{
				Write("Error in Command\n");
				Write(luaL_checkstring( ScriptManager::Get()->GetState()/*g_Console->m_pScriptContext->GetScriptContext()*/, -1));
			}
			// clear buffer when done processing
			memset(m_CommandBufferW, 0, 4096*sizeof(wchar_t));
			break;
    }

    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
//--------------------------------------------------------------------------------
void ConsoleWindow::Write(const char *pString)
{
	if(g_Console && m_hWnd)
	{
		//remove any linefeed chars (look goofy in log)
		char *buf = new char[strlen(pString)+1];
		int indx=0;
		for(int i=0; i<(int) strlen(pString); i++)
		{
			if(pString[i] != 10)
			{
				buf[indx++] = pString[i];
			}
			else
			{
				buf[indx] = 0;
				g_Console->m_stringList.push_front( buf );
				indx = 0;
			}


		}
		if(indx > 0)
		{
			buf[indx] = 0;
			g_Console->m_stringList.push_front( buf );
		}
		
		InvalidateRect(m_hWnd, NULL, TRUE);
		delete[] buf;

		g_Console->AdjustScrollBar();
	}


}
//--------------------------------------------------------------------------------
LRESULT CALLBACK ConsoleWindow::SubclassInputEditProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) 
{     
	switch( message )     
	{         
	case WM_CHAR:              
		if ( (TCHAR)wParam == VK_RETURN ) // Found a RETURN keystroke!             
		{ 
			// get the command string
			long lSizeofString;

			// Get the size of the string
			lSizeofString = SendMessage( hWnd, WM_GETTEXTLENGTH, 0, 0 );

            // Get the string                 
			SendMessage( hWnd, WM_GETTEXT, lSizeofString + 1, (LPARAM) m_CommandBufferW ); 

			// send message to parent that command was entered
			SendMessage( m_hWnd, WM_USER, 0, lSizeofString );

			// clear the edit string
			SendMessage( hWnd, WM_SETTEXT, 0, (long) "" ); 
			return 1;
		}
	}
    
	return CallWindowProc( lpfnInputEdit, hWnd, message, wParam, lParam ); 
}
//--------------------------------------------------------------------------------
void ConsoleWindow::Init( HINSTANCE hInstance )
{
 	m_hInstance = hInstance;
	m_ScrollyPos = 0;

    // create application handler and link to our WindowProc
    WNDCLASS wc;

    // Set up and register window class
    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC) MsgProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(DWORD);
    wc.hInstance = m_hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"ConsoleWindow";

	RegisterClass( &wc );

    m_hWnd = CreateWindow( L"ConsoleWindow",	// class
                        L"LUA WinConsole",		// caption
                        WS_OVERLAPPEDWINDOW,	// style 
                        700,					// left
                        400,					// top
                        480,					// width
                        240,					// height
                        NULL,					// parent window
                        NULL,					// menu 
                        m_hInstance,			// instance
                        NULL );					// parms

    ShowWindow( m_hWnd, SW_SHOW );
    UpdateWindow( m_hWnd );
    SetFocus( m_hWnd );

	m_hEditControl = CreateWindow( L"EDIT",	// class
                        L"",					// caption
                        ES_LEFT | WS_CHILD, // style 
                        2,					// left
                        404,				// top
                        228,				// width
                        16,					// height
                        m_hWnd,				// parent window
                        (HMENU)0xa7,		// menu 
                        m_hInstance,		// instance
                        NULL );				// parms

    ShowWindow( m_hEditControl, SW_SHOW );
    UpdateWindow( m_hEditControl );
    SetFocus( m_hEditControl );
	m_ScrollyPos = 0;

	lpfnInputEdit = (WNDPROC)SetWindowLongPtr( m_hEditControl, GWLP_WNDPROC, (long) SubclassInputEditProc ); 
	g_Console->ResizeControls();


}
//--------------------------------------------------------------------------------
static int Debug_Print(lua_State *L) 
{
#ifdef _DEBUG
	
	int n = lua_gettop( L );  /* number of arguments */
	
	lua_getglobal( L, "tostring" );
	
	for ( int i = 1; i <= n; i++ )
	{
		const char *s;
		lua_pushvalue( L, -1 );  /* function to be called */
		lua_pushvalue( L, i );   /* value to print */
		lua_call( L, 1, 1 );
		s = lua_tostring( L, -1 );  /* get result */
		
		if (s == NULL)
			return luaL_error( L, "`tostring' must return a string to `print'" );
		
		if ( i > 1 )
			ConsoleWindow::Write( "\t" );

		ConsoleWindow::Write( s );
		lua_pop( L, 1 );  /* pop result */
	}

	ConsoleWindow::Write( "\n" );

#endif
	return 0;
}
//--------------------------------------------------------------------------------
luaL_reg DebugGlue[] = 
{
  {"Print",					Debug_Print},
  {"print",					Debug_Print},

  {NULL, NULL}
};
//--------------------------------------------------------------------------------
HWND ConsoleWindow::GetHandle()
{
	return( ConsoleWindow::m_hWnd );
}
//--------------------------------------------------------------------------------