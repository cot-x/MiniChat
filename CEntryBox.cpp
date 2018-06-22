#include "CEntryBox.h"


#define UM_DownReturn	WM_USER+1

// private //

std::map<HWND,WNDPROC>	CEntryBox::sm_proc;


LRESULT CEntryBox::OnCreate( HWND hWnd, UINT, WPARAM, LPARAM lParam )
{
	WNDPROC	OrgProc;


	m_hEdit = ::CreateWindowEx( WS_EX_CLIENTEDGE,
		TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
		0, 0, 0, 0, hWnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL );
	if( NULL == m_hEdit ){
		::DestroyWindow( hWnd );
		return -1;
	}
	::SendMessage( m_hEdit, EM_LIMITTEXT, m_lLimit, 0 );

	OrgProc = (WNDPROC)::GetWindowLong( m_hEdit, GWL_WNDPROC );
	sm_proc.insert(std::pair<HWND,WNDPROC>(m_hEdit, OrgProc));
	::SetWindowLong( m_hEdit, GWL_WNDPROC, (LONG)CEntryBox::EditProc );

	return 0;
}

LRESULT CEntryBox::OnSize( HWND, UINT, WPARAM, LPARAM lParam )
{
	::MoveWindow( m_hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE );

	return 0;
}

LRESULT CEntryBox::OnCtlColorEdit( HWND, UINT, WPARAM wParam, LPARAM )
{
	::SetTextColor( (HDC)wParam, m_TextColor );
	::SetBkColor( (HDC)wParam, m_TextBkColor );

	return (LRESULT)m_hBrush;
}

LRESULT CEntryBox::OnSetFocus( HWND, UINT, WPARAM, LPARAM )
{
	::SetFocus( m_hEdit );

	return 0;
}

bool CEntryBox::ExtraMessageProc( HWND, UINT Msg, WPARAM, LPARAM )
{
	switch( Msg ){
	case UM_DownReturn:{
		TCHAR	*szText;
		int	len;
	
		len = ::GetWindowTextLength( m_hEdit );
		if( 0 == len )
			return false;
	
		szText = new TCHAR[len+1];
		if( NULL == szText ){
			::MessageBox( hWnd(),
				TEXT("メモリの確保に失敗しました。"), NULL, 0 );
			return false;
		}
	
		if( !::GetWindowText(m_hEdit, szText, len+1) ){
			::MessageBox( hWnd(), TEXT("テキストの取得に失敗しました。"),
				NULL, MB_ICONINFORMATION );
			delete szText;
			return false;
		}
	
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)szText );
		delete szText;
		::SetWindowText( m_hEdit, TEXT("") );
	
		return true;
		}
	}

	return false;
}

LRESULT CALLBACK CEntryBox::EditProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	std::map<HWND,WNDPROC>::iterator	p;


	p = sm_proc.find(hWnd);
	if( p == sm_proc.end() )
		return 0;

	switch( Msg ){
	case WM_KEYDOWN:
		switch( wParam ){
		case VK_RETURN:
			::SendMessage( ::GetParent(hWnd), UM_DownReturn, 0, 0 );
			return 0;
		}
	case WM_CHAR:
		if( '\n' == wParam || '\r' == wParam )
			return 0;
	}

	return ::CallWindowProc( p->second, hWnd, Msg, wParam, lParam );
}


// public //

CEntryBox::CEntryBox( HINSTANCE hInstance, TCHAR *name, HWND hWndParent,
	long lLimit, UINT Msg,
	COLORREF TextColor, COLORREF TextBkColor, HBRUSH hBrush )
: CWnd( hInstance, name, WS_CHILD, hWndParent ),
	m_hWndParent(hWndParent), m_lLimit(lLimit), m_Msg(Msg),
	m_TextColor(TextColor), m_TextBkColor(TextBkColor), m_hBrush(hBrush)
{
	OnCreate( hWnd(), WM_CREATE, 0, (LPARAM)&m_cs );
}

CEntryBox::~CEntryBox()
{
	std::map<HWND,WNDPROC>::iterator	p;


	p = sm_proc.find(m_hEdit);
	if( p != sm_proc.end() )
		sm_proc.erase(p);
}

CEntryBox & CEntryBox::operator =( const CEntryBox & )
{
	return *this;
}

LONG CEntryBox::Height()
{
	TEXTMETRIC	tm;
	HDC		hdc;

	hdc = ::CreateIC( TEXT("DISPLAY"), NULL, NULL, NULL );
	::GetTextMetrics( hdc, &tm );

	return tm.tmHeight + ::GetSystemMetrics(SM_CYEDGE)*2;
}
