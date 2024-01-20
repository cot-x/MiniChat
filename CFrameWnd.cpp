#include "CFrameWnd.h"

#include "Resource.h"


#define BGColor		RGB(0,0,0)
#define	TitleTextColor	RGB(0xFF,0xFF,0xFF)
#define TextColor	RGB(0xFF,0xFF,0xFF)
#define TitleColor 	RGB(32,32,64)
#define CloseColor1	RGB(64,64,96)
#define CloseColor2	RGB(96,96,128)

#define ENTERLIMIT	100
#define HNLIMIT		30
#define DEFPORT		TEXT("1000")
#define UM_ENTERTEXT	WM_USER+1
#define UM_CHAT		WM_USER+2
#define DLG_CANCEL	0
#define DLG_CLIENT	1
#define DLG_SERVER	2


// private //

bool CFrameWnd::ExtraMessageProc( HWND, UINT Msg, WPARAM, LPARAM lParam )
{
	switch( Msg ){
	case UM_ENTERTEXT:
		if( m_pcMiniChat->CheckCommand((TCHAR *)lParam) ){
			m_pcMiniChat->PlaySound();
			return true;
		}
	
		if( m_bServer ){
			TCHAR	szCmd4[5];
		
			m_pcMiniChat->SendChat( m_sNetSelect.szHN );
			m_pcMsgEdit->SetMsg( m_sNetSelect.szHN );
			m_pcMiniChat->SendChat("> ");
			m_pcMsgEdit->SetMsg( TEXT("> ") );
			m_pcMsgEdit->SetMsg( (TCHAR *)lParam );
			m_pcMsgEdit->SetMsg( TEXT("\r\n") );
			m_pcMiniChat->PlaySound();
		
			::lstrcpyn( szCmd4, (TCHAR *)lParam, 5 );
			if( m_bServer && ::lstrcmp(szCmd4, TEXT("/HN ")) == 0 ){
				TCHAR	*szHN, *szTemp;
				int	nLen;
				bool	bChange;
			
				m_pcMiniChat->SendChat((TCHAR *)lParam);
				m_pcMiniChat->SendChat("\r\n");
			
				szHN = (TCHAR *)lParam+4;
			
				bChange = !m_pcMiniChat->CheckHN( szHN );
				if( bChange ){
					nLen = ::lstrlen( szHN );
					szTemp = new TCHAR[nLen+1];
					if( szTemp ){
						if( !::lstrcpy(szTemp, szHN) ){
							delete szTemp;
							return true;
						}
					}else{
						return true;
					}
				}else{
					m_pcMiniChat->SendChat( "すでに同じ名前が使われて"
								"いるため使用できません。\r\n" );
					m_pcMsgEdit->SetMsg( "すでに同じ名前が使われて"
								"いるため使用できません。\r\n" );
				}
			
				if( bChange ){
					delete m_sNetSelect.szHN;
					m_sNetSelect.szHN = szTemp;
					m_pcMiniChat->ChangeHN( m_sNetSelect.szHN );
					m_pcMiniChat->SendChat( "HNを変更しました\r\n" );
					m_pcMsgEdit->SetMsg( "HNを変更しました\r\n" );
				}
			
				return true;
			}
		}
		m_pcMiniChat->SendChat( (char *)lParam );
		m_pcMiniChat->SendChat("\r\n");
	
		return true;
	
	case UM_CHAT:
		m_pcMsgEdit->SetMsg( (TCHAR *)lParam );
		return true;
	}

	return false;
}

LRESULT CFrameWnd::OnSetFocus( HWND, UINT, WPARAM, LPARAM )
{
	::SetFocus( m_pcEntryBox->hWnd() );

	return 0;
}

BOOL CALLBACK CFrameWnd::DialogProc( HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	static int		nRet = DLG_CLIENT;
	static SNetSelect	*psNetSelect;


	switch( Msg ){
	case WM_INITDIALOG:{
		HWND	hEdit;
	
		::CheckRadioButton( hDlg, IDC_CLIENT, IDC_SERVER, nRet+999 );
	
		hEdit = ::GetDlgItem( hDlg, IDC_PORT );
		::SendMessage( hEdit, EM_LIMITTEXT, 5, 0 );
		::SetWindowText( hEdit, DEFPORT );
	
		psNetSelect = (SNetSelect *)lParam;
		return TRUE;
		}
	
	case WM_COMMAND:
		switch( LOWORD(wParam) ){
		case IDOK:{
			HWND	hEdit;
			int	len, nPort;
			TCHAR	*szText;
			BOOL	bTranslate;
		
			hEdit = ::GetDlgItem( hDlg, IDC_HN );
			len = ::GetWindowTextLength( hEdit );
			if( 0 == len ){
				::MessageBox( hDlg, TEXT("HNを入力してください。"),
					TEXT("Enter your HN!"), MB_ICONINFORMATION );
				return TRUE;
			}
			szText = new TCHAR[len+1];
			if( NULL == szText ){
				::MessageBox( hDlg,
					TEXT("メモリの確保に失敗しました。"), NULL, 0 );
				EndDialog( hDlg, DLG_CANCEL );
				return TRUE;
			}
			if( !::GetWindowText(hEdit, szText, len+1) ){
				::MessageBox( hDlg, TEXT("HNの取得に失敗しました。"),
					NULL, MB_ICONERROR );
				delete szText;
				EndDialog( hDlg, DLG_CANCEL );
				return TRUE;
			}
			psNetSelect->szHN = szText;
		
			hEdit = ::GetDlgItem( hDlg, IDC_PORT );
			len = ::GetWindowTextLength( hEdit );
			if( 0 == len ){
				::MessageBox( hDlg, TEXT("PORT番号を入力してください。"),
					TEXT("PORT"), MB_ICONINFORMATION );
				return TRUE;
			}
			nPort = ::GetDlgItemInt( hDlg, IDC_PORT, &bTranslate, FALSE );
			if( !bTranslate ){
				::MessageBox( hDlg, TEXT("PORTの取得に失敗しました。"),
					NULL, MB_ICONERROR );
				EndDialog( hDlg, DLG_CANCEL );
				return TRUE;
			}
			if( nPort > 0xFFFF ){	// u_short
				::MessageBox( hDlg, TEXT("0～65535までの値で入力してください。"),
					TEXT("PORT"), MB_ICONINFORMATION );
				return TRUE;
			}
			psNetSelect->nPort = (u_short)nPort;
		
			if( DLG_SERVER == nRet ){
				::EndDialog( hDlg, nRet );
				return TRUE;
			}
		
			hEdit = ::GetDlgItem( hDlg, IDC_ADDR );
			len = ::GetWindowTextLength( hEdit );
			if( 0 == len ){
				::MessageBox( hDlg, TEXT("サーバーのアドレスを入力してください。"),
					TEXT("CLIENT"), MB_ICONINFORMATION );
				return TRUE;
			}
			szText = new TCHAR[len+1];
			if( NULL == szText ){
				::MessageBox( hDlg,
					TEXT("メモリの確保に失敗しました。"), NULL, 0 );
				EndDialog( hDlg, DLG_CANCEL );
				return TRUE;
			}
			if( !::GetWindowText(hEdit, szText, len+1) ){
				::MessageBox( hDlg, TEXT("アドレスの取得に失敗しました。"),
					NULL, MB_ICONERROR );
				delete szText;
				EndDialog( hDlg, DLG_CANCEL );
				return TRUE;
			}
			psNetSelect->szAddr = szText;
		
			::EndDialog( hDlg, nRet );
			return TRUE;
			}
		
		case IDCANCEL:
			::EndDialog( hDlg, DLG_CANCEL );
			return TRUE;
		
		case IDC_CLIENT:
			nRet = DLG_CLIENT;
			return TRUE;
		
		case IDC_SERVER:
			nRet = DLG_SERVER;
			return TRUE;
		}
		break;
	}

	return FALSE;
}

void CFrameWnd::OnFrameCreate( HWND hWnd, UINT, WPARAM, LPARAM lParam )
{
	m_pcMsgEdit = new CMsgEdit( ((LPCREATESTRUCT)lParam)->hInstance,
		TEXT("m_pcMsgEdit"), hWnd, RGB(0,0xFF,0), RGB(0,0,0),
		(HBRUSH)::GetStockObject(BLACK_BRUSH) );
	if( m_pcMsgEdit->hWnd() == NULL ){
		::MessageBox( hWnd, TEXT("ウィンドウの作成に失敗しました。"), NULL, 0 );
		::DestroyWindow( hWnd );
		return;
	}
	m_pcMsgEdit->Show( SW_SHOW );

	m_pcEntryBox = new CEntryBox( ((LPCREATESTRUCT)lParam)->hInstance,
		TEXT("m_pcEntryBox"), hWnd, ENTERLIMIT, UM_ENTERTEXT,
		RGB(0,0xFF,0), RGB(0,0,0), (HBRUSH)::GetStockObject(BLACK_BRUSH) );
	if( m_pcEntryBox->hWnd() == NULL ){
		::MessageBox( hWnd, TEXT("ウィンドウの作成に失敗しました。"), NULL, 0 );
		::DestroyWindow( hWnd );
		return;
	}
	m_pcEntryBox->Show( SW_SHOW );

	m_hBitmap = ::LoadBitmap( ((LPCREATESTRUCT)lParam)->hInstance, TEXT("BG_BMP") );
	if( m_hBitmap )
		::GetObject( m_hBitmap, sizeof(m_bitmap), &m_bitmap );
}

void CFrameWnd::OnFrameDestroy( HWND, UINT, WPARAM, LPARAM )
{
	if( m_pcMsgEdit )
		delete m_pcMsgEdit;
	if( m_pcEntryBox )
		delete m_pcEntryBox;
	if( m_hBitmap )
		::DeleteObject( m_hBitmap );
}

void CFrameWnd::OnFrameSize( HWND, RECT *pRect, int )
{
	pRect->bottom -= CEntryBox::Height() * 3/2;

	m_pcMsgEdit->Move( pRect->left, pRect->top,
		pRect->right-pRect->left, pRect->bottom-pRect->top );
	
	pRect->bottom += CEntryBox::Height() * 1/2;

	m_pcEntryBox->Move( pRect->left, pRect->bottom,
		pRect->right-pRect->left, CEntryBox::Height() );
}

void CFrameWnd::OnFramePaint( HWND hWnd, HDC hdc )
{
	SIZE	size;
	RECT	rect;
	TCHAR	szSign[] = TEXT("Copyright: 流");


	::GetClientRect( hWnd, &rect );

	if( m_hBitmap && ::GetDeviceCaps(hdc, RASTERCAPS) & RC_STRETCHBLT ){
		HDC	hdcMem;
	
		hdcMem = ::CreateCompatibleDC( hdc );
		if( !hdcMem )
			return;
		::SelectObject( hdcMem, m_hBitmap );
		::StretchBlt( hdc, 0, m_YCaption, rect.right, rect.bottom,
			hdcMem, 0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight, SRCCOPY );
		::DeleteDC( hdcMem );
	}

	::GetTextExtentPoint32( hdc, szSign, ::lstrlen(szSign), &size );
	::TextOut( hdc, rect.right-size.cx-m_round, rect.bottom-size.cy-m_round/2,
		szSign, ::lstrlen(szSign) );
}

void CFrameWnd::OnPaint( HWND hWnd, HDC hdc )
{
	SIZE	size;
	int	len;
	TCHAR	*szTitle;
	RECT	rect;
	HBRUSH	hBrush;


	size.cy	= 0;
	len	= ::GetWindowTextLength( hWnd );
	szTitle	= new TCHAR[len+1];
	if( NULL != szTitle )
		len = ::GetWindowText( hWnd, szTitle, len+1 );
		if( 0 != len )
			::GetTextExtentPoint32( hdc, szTitle, len, &size );

	::GetClientRect( hWnd, &rect );
	if( 0 == size.cy )
		rect.bottom = ::GetSystemMetrics( SM_CYCAPTION );
	else
		rect.bottom = size.cy + ::GetSystemMetrics( SM_CYFIXEDFRAME )*2;

	m_YCaption = rect.bottom;
	m_XCaption = rect.right;

	hBrush = ::CreateSolidBrush( TitleColor );
	::FillRect( hdc, &rect, hBrush );
	DeleteObject( hBrush );
	if( !m_bClosing )
		hBrush = ::CreateSolidBrush( CloseColor1 );
	else
		hBrush = ::CreateSolidBrush( CloseColor2 );
	rect.left = rect.right - m_round;
	::FillRect( hdc, &rect, hBrush );
	DeleteObject( hBrush );
	rect.left = 0;

	if( NULL != szTitle ){
		rect.left	+= m_round;
		rect.right	-= m_round;
		::SetBkColor( hdc, TitleColor );
		::SetTextColor( hdc, TitleTextColor );
		::DrawText( hdc, szTitle, -1, &rect,
			DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS );
	}

	::SetBkColor( hdc, BGColor );
	::SetTextColor( hdc, TextColor );
	OnFramePaint( hWnd, hdc );
}

LRESULT CFrameWnd::OnCreate( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	static HBRUSH	hBrush;


	hBrush = ::CreateSolidBrush( BGColor );
	if( NULL != hBrush )
		if( !::SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)hBrush) )
			DeleteObject( hBrush );

	OnFrameCreate( hWnd, Msg, wParam, lParam );

	return 0;
}

LRESULT CFrameWnd::OnDestroy( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	OnFrameDestroy( hWnd, Msg, wParam, lParam );

	::PostQuitMessage(0);

	return 0;
}

LRESULT CFrameWnd::OnSize( HWND hWnd, UINT, WPARAM, LPARAM )
{
	RECT	rect;
	HRGN	hRgn;
	int	Caption, XFrame, YFrame;


	Caption	= ::GetSystemMetrics( SM_CYCAPTION );
	XFrame	= ::GetSystemMetrics( SM_CXFIXEDFRAME );
	YFrame	= ::GetSystemMetrics( SM_CYFIXEDFRAME );

	::GetWindowRect( hWnd, &rect );
	m_nHeight	= rect.bottom - rect.top;
	m_nWidth	= rect.right - rect.left;
	rect.bottom	= (rect.bottom - rect.top) - YFrame;
	rect.right	= (rect.right - rect.left) - XFrame;
	rect.top	= Caption + YFrame;
	rect.left	= XFrame;
	m_round		= min( rect.bottom/10, rect.right/10 );
	hRgn = ::CreateRoundRectRgn( rect.left, rect.top,
		rect.right, rect.bottom, m_round, m_round );
	::SetWindowRgn( hWnd, hRgn, TRUE );

	::UpdateWindow( hWnd );	// m_YCaption更新の為
	::GetClientRect( hWnd, &rect );
	rect.top	+= max( m_YCaption, m_round );
	rect.bottom	-= m_round;
	rect.left	+= m_round;
	rect.right	-= m_round;
	OnFrameSize( hWnd, &rect, m_round );

	return 0;
}

LRESULT CFrameWnd::OnLButtonDown( HWND hWnd, UINT, WPARAM, LPARAM lParam )
{
	::SetCapture( hWnd );

	if( HIWORD(lParam) <= m_YCaption ){
		if( LOWORD(lParam) >= (m_XCaption-m_round) ){
			RECT	rect;
		
			m_bClosing = true;
			rect.left	= m_XCaption - m_round;
			rect.top	= 0;
			rect.right	= m_XCaption;
			rect.bottom	= m_YCaption;
			::InvalidateRect( hWnd, &rect, FALSE );
		}else{
			POINT	ClientPos;
			RECT	rect;
		
			m_bMoving = true;
			m_pos.x = LOWORD(lParam);
			m_pos.y = HIWORD(lParam);
		
			ClientPos.x = ClientPos.y = 0;
			::ClientToScreen( hWnd, &ClientPos );
			::GetWindowRect( hWnd, &rect );
		
			m_pos.x += ClientPos.x - rect.left;
			m_pos.y += ClientPos.y - rect.top;
		}
	}

	return 0;
}

LRESULT CFrameWnd::OnLButtonUp( HWND hWnd, UINT, WPARAM, LPARAM lParam )
{
	::ReleaseCapture();

	if( m_bClosing ){
		RECT	rect;
	
		m_bClosing = false;
		rect.left	= m_XCaption - m_round;
		rect.top	= 0;
		rect.right	= m_XCaption;
		rect.bottom	= m_YCaption;
		::InvalidateRect( hWnd, &rect, FALSE );
	
		if( HIWORD(lParam) <= rect.bottom &&
			LOWORD(lParam) >= rect.left &&
			HIWORD(lParam) >= rect.top &&
			LOWORD(lParam) <= rect.right )
				::DestroyWindow( hWnd );
	}
	if( m_bMoving )
		m_bMoving = false;

	return 0;
}

LRESULT CFrameWnd::OnMouseMove( HWND, UINT, WPARAM, LPARAM )
{
	if( m_bMoving ){
		POINT	pos;
	
		::GetCursorPos( &pos );
		pos.x -= m_pos.x;
		pos.y -= m_pos.y;
	
		Move( pos.x, pos.y, m_nWidth, m_nHeight );
	}

	return 0;
}

LRESULT CFrameWnd::OnSetText( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	::InvalidateRect( hWnd, NULL, TRUE );

	return ::DefWindowProc( hWnd, Msg, wParam, lParam );
}


// public //

CFrameWnd::CFrameWnd( HINSTANCE hInstance, TCHAR *title, HICON hIcon )
: CWnd( hInstance, title,
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN, NULL, hIcon ),
	m_bClosing(false), m_bMoving(false), m_bServer(true), m_hBitmap(NULL)
{
	m_sNetSelect.szAddr	= NULL;
	m_sNetSelect.szHN	= NULL;
	OnCreate( hWnd(), WM_CREATE, 0, (LPARAM)&m_cs );
}

CFrameWnd::~CFrameWnd()
{
	if( m_sNetSelect.szAddr )
		delete m_sNetSelect.szAddr;
	if( m_sNetSelect.szHN )
		delete m_sNetSelect.szHN;
}

CFrameWnd & CFrameWnd::operator =( const CFrameWnd & )
{
	return *this;
}

void CFrameWnd::SelectNet( HINSTANCE hInstance )
{
	int	nRet;


	nRet = ::DialogBoxParam( hInstance, TEXT("NetSelect"), hWnd(),
		CFrameWnd::DialogProc, (LPARAM)&m_sNetSelect );
	switch( nRet ){
	case DLG_CANCEL:
		::DestroyWindow( hWnd() );
		return;
	case DLG_CLIENT:
		m_bServer = false;
		break;
	case DLG_SERVER:
		m_bServer = true;
		break;
	case -1:
	default:
		::MessageBox( hWnd(), TEXT("ダイアログでエラーが発生しました。"), NULL, 0 );
		::DestroyWindow( hWnd() );
		return;
	}

	m_pcMiniChat = new CMiniChat( hInstance, TEXT("m_pcMiniChat"), hWnd(), UM_CHAT,
		m_sNetSelect.szHN, m_bServer, m_sNetSelect.szAddr, m_sNetSelect.nPort );
	if( NULL == m_pcMiniChat->hWnd() ){
		::MessageBox( hWnd(), TEXT("ネットワークのスタートに失敗しました。"), NULL, 0 );
		::DestroyWindow( hWnd() );
	}
}
