#include "CWnd.h"


// private //

CWnd	*CWnd::sm_this = NULL;


#define CALL( message, method )	case message:	return method( m_hWnd, Msg, wParam, lParam );
LRESULT CWnd::Dispatch( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	if( ExtraMessageProc(hWnd, Msg, wParam, lParam) )
		return 0;

	switch( Msg ){
		CALL( WM_PAINT,			WMPaint )
		CALL( WM_DESTROY,		OnDestroy )
		CALL( WM_QUIT,			OnQuit )
		CALL( WM_CREATE,		OnCreate )
		CALL( WM_COMMAND,		OnCommand )
		CALL( WM_SYSCOMMAND,		OnSysCommand )
		CALL( WM_ACTIVATE,		OnActivate )
		CALL( WM_ACTIVATEAPP,		OnActivateApp )
		CALL( WM_SETFOCUS,		OnSetFocus )
		CALL( WM_SETTEXT,		OnSetText )
		CALL( WM_WINDOWPOSCHANGED,	OnWindowPosChanged )
		CALL( WM_WINDOWPOSCHANGING,	OnWindowPosChanging )
		CALL( WM_SIZE,			OnSize )
		CALL( WM_SIZING,		OnSizing )
		CALL( WM_MOVE,			OnMove )
		CALL( WM_MOVING,		OnMoving )
		CALL( WM_ENABLE,		OnEnable )
		CALL( WM_DROPFILES,		OnDropFiles )
		CALL( WM_CHAR,			OnChar )
		CALL( WM_KEYDOWN,		OnKeyDown )
		CALL( WM_KEYUP,			OnKeyUp )
		CALL( WM_SYSCHAR,		OnSysChar )
		CALL( WM_SYSKEYDOWN,		OnSysKeyDown )
		CALL( WM_SYSKEYUP,		OnSysKeyUp )
		CALL( WM_KILLFOCUS,		OnKillFocus )
		CALL( WM_MENUSELECT,		OnMenuSelect )
		CALL( WM_MOUSEMOVE,		OnMouseMove )
		CALL( WM_MOUSEWHEEL,		OnMouseWheel )
		CALL( WM_LBUTTONDBLCLK,		OnLButtonDBLCLK )
		CALL( WM_LBUTTONDOWN,		OnLButtonDown )
		CALL( WM_LBUTTONUP,		OnLButtonUp )
		CALL( WM_MBUTTONDBLCLK,		OnMButtonDBLCLK )
		CALL( WM_MBUTTONDOWN,		OnMButtonDown )
		CALL( WM_MBUTTONUP,		OnMButtonUp )
		CALL( WM_RBUTTONDBLCLK,		OnRButtonDBLCLK )
		CALL( WM_RBUTTONDOWN,		OnRButtonDown )
		CALL( WM_RBUTTONUP,		OnRButtonUp )
		CALL( WM_TIMER,			OnTimer )
		CALL( WM_DRAWITEM,		OnDrawItem )
		CALL( WM_HSCROLL,		OnHScroll )
		CALL( WM_VSCROLL,		OnVScroll )
		CALL( WM_CTLCOLORBTN,		OnCtlColorBtn )
		CALL( WM_CTLCOLORDLG,		OnCtlColorDlg )
		CALL( WM_CTLCOLOREDIT,		OnCtlColorEdit )
		CALL( WM_CTLCOLORLISTBOX,	OnCtlColorListBox )
		CALL( WM_CTLCOLORMSGBOX,	OnCtlColorMsgBox )
		CALL( WM_CTLCOLORSCROLLBAR,	OnCtlColorScrollBar )
		CALL( WM_CTLCOLORSTATIC,	OnCtlColorStatic )
		CALL( WM_QUERYENDSESSION,	OnQueryEndSession )
	}

	return ::DefWindowProc( hWnd, Msg, wParam, lParam );
}

LRESULT CALLBACK CWnd::WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	PCWnd	pCWnd;


	if( sm_this != NULL )
		return sm_this->Dispatch( hWnd, Msg, wParam, lParam );

	if( 0 == (pCWnd = (PCWnd)::GetWindowLong(hWnd, 0)) )
		return ::DefWindowProc( hWnd, Msg, wParam, lParam );

	return pCWnd->Dispatch( hWnd, Msg, wParam, lParam );
}

LRESULT CWnd::WMPaint( HWND hWnd, UINT, WPARAM, LPARAM )
{
	PAINTSTRUCT	ps;
	HDC		hdc = BeginPaint( hWnd, &ps );

	OnPaint( hWnd, hdc );

	EndPaint( hWnd, &ps );

	return 0;
}


// public //

CWnd::CWnd( HINSTANCE hInstance, TCHAR *title, UINT style,
		HWND hWndParent, HICON hIcon )	// hWndParent, hIconはデフォルト引数あり
: m_hInstance(hInstance), m_hWnd(NULL)
{
	WNDCLASS	wc;


	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 4;	// メッセージ処理に使用
	wc.hInstance		= m_hInstance;
	wc.hIcon		= hIcon;
	wc.hCursor		= LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= title;
	if( !::RegisterClass(&wc) )
		throw -1;

	if( sm_this != NULL )
		throw -2;

	sm_this = this;

	m_hWnd = ::CreateWindow( title, title, style,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hWndParent, NULL, m_hInstance, NULL );

	if( m_hWnd == NULL ){
		sm_this = NULL;
		throw -3;
	}

	::SetWindowLong( m_hWnd, 0, (LONG)sm_this );
	sm_this = NULL;
}

CWnd::~CWnd()
{
	if( m_hWnd )
		::DestroyWindow( m_hWnd );
}

CWnd::CWnd( const CWnd & )
{
	throw -4;	// コピー禁止
}

CWnd & CWnd::operator =( const CWnd & )
{
	return *this;	// 代入禁止
}

HWND CWnd::hWnd()
{
	return m_hWnd;
}

BOOL CWnd::Show( int nCmdShow )
{
	return ::ShowWindow( m_hWnd, nCmdShow );
}

BOOL CWnd::Move( int X, int Y, int nWidth, int nHeight )
{
	return ::MoveWindow( m_hWnd, X, Y, nWidth, nHeight, TRUE );
}

BOOL CWnd::Menu( HMENU hMenu )
{
	return ::SetMenu( m_hWnd, hMenu );
}

void CWnd::Cursor( LPCTSTR lpCursorName, bool bInstance )
{
	if( bInstance )
		::SetCursor( ::LoadCursor(m_hInstance, lpCursorName) );
	else
		::SetCursor( ::LoadCursor(NULL, lpCursorName) );
}

WPARAM CWnd::MessageLoop()
{
	MSG	msg;
	BOOL	cmsg;


	while( (cmsg = ::GetMessage(&msg, NULL, 0, 0)) != 0 ){
		if( -1 == cmsg ){
			::MessageBox( NULL, TEXT("MessageLoop"), NULL, 0 );
			return (WPARAM)-1;
		}else{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
	}

	return msg.wParam;
}
