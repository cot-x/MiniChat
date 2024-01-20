// MiniChat Ver.2.1.2
//	簡易多人数ミニチャット
//
//		サーバー・クライアント型チャットソフト(TCPでIPアドレス指定型)
//		PlaySound()を使うためWinmm.libをリンクしてやる必要がある

#include "CFrameWnd.h"

#define APPNAME	TEXT("MiniChat Ver.2.1.2")


int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE, LPSTR, int )
{
	try{
		RECT		rect;
		CFrameWnd	window( hInstance, APPNAME,
					::LoadIcon(hInstance, TEXT("MChat_ICO")) );
	
		::GetWindowRect( window.hWnd(), &rect );
		window.Move( rect.left, rect.top, 640, 480 );
		window.Show( SW_SHOW );
		window.SelectNet( hInstance );
	
		return CWnd::MessageLoop();
	}catch( int e ){
		TCHAR	szText[3];
		::wsprintf( szText, TEXT("%d"), e );
		::MessageBox( NULL, szText, TEXT("例外"), 0 );
	}
	return -1;
}