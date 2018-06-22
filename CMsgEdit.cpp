#include "CMsgEdit.h"


#define LIMITTEXT	0xFFFFFFFF

// private //

LRESULT CMsgEdit::OnCreate( HWND hWnd, UINT, WPARAM, LPARAM lParam )
{
	m_hEdit = ::CreateWindowEx( WS_EX_STATICEDGE,
		TEXT("EDIT"), NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
		ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_MULTILINE | ES_READONLY,
		0, 0, 0, 0, hWnd, NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL );
	if( NULL == m_hEdit ){
		::DestroyWindow( hWnd );
		return -1;
	}
	::SendMessage( m_hEdit, EM_LIMITTEXT, LIMITTEXT, 0 );

	return 0;
}

LRESULT CMsgEdit::OnSize( HWND, UINT, WPARAM, LPARAM lParam )
{
	::MoveWindow( m_hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE );

	return 0;
}

LRESULT CMsgEdit::OnCtlColorStatic( HWND, UINT, WPARAM wParam, LPARAM )
{
	::SetTextColor( (HDC)wParam, m_TextColor );
	::SetBkColor( (HDC)wParam, m_TextBkColor );

	return (LRESULT)m_hBrush;
}

LRESULT CMsgEdit::OnSetFocus( HWND, UINT, WPARAM, LPARAM )
{
	::SetFocus( m_hEdit );

	return 0;
}


// public //

CMsgEdit::CMsgEdit( HINSTANCE hInstance, TCHAR *name, HWND hWndParent,
	COLORREF TextColor, COLORREF TextBkColor, HBRUSH hBrush )
: CWnd( hInstance, name, WS_CHILD, hWndParent ),
	m_lLen(0),
	m_TextColor(TextColor), m_TextBkColor(TextBkColor), m_hBrush(hBrush)
{
	OnCreate( hWnd(), WM_CREATE, 0, (LPARAM)&m_cs );
}

CMsgEdit::~CMsgEdit()
{
}

CMsgEdit & CMsgEdit::operator =( const CMsgEdit & )
{
	return *this;
}

bool CMsgEdit::SetMsg( TCHAR *szText )
{
	int	nStart, nEnd, len;


	len = ::lstrlen( szText );
	if( !(LIMITTEXT >= (unsigned long)len) )
		return false;
	if( !(LIMITTEXT >= (unsigned long)(m_lLen+len)) ){
		int	nOffset = 0;
	
		for( int i=0, nRet;
			!(LIMITTEXT >= (unsigned long)(m_lLen+len-nOffset)); i++ ){
			nRet = ::SendMessage( m_hEdit, EM_LINEINDEX, i, 0 );
			if( nRet == -1 )
				return false;
			nOffset += nRet;
		}
	
		::SendMessage( m_hEdit, EM_SETSEL, 0, nOffset+1 );
		::SendMessage( m_hEdit, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)TEXT("") );
	
		m_lLen -= nOffset;
	}

	::SendMessage( m_hEdit, EM_SETSEL, 0, -1 );
	::SendMessage( m_hEdit, EM_GETSEL, (WPARAM)&nStart, (LPARAM)&nEnd );
	::SendMessage( m_hEdit, EM_SETSEL, nEnd, nEnd );
	::SendMessage( m_hEdit, EM_REPLACESEL, (WPARAM)FALSE, (LPARAM)szText );
	::SendMessage( m_hEdit, WM_HSCROLL, MAKEWPARAM(0,SB_TOP), NULL );

	m_lLen += len;

	return true;
}
