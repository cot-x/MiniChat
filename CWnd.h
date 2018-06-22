// CWnd
//	ウィンドウ基底クラス
//
//		オブジェクト作成と同時にウィンドウを作成
//		作成失敗時には例外発生
//		オブジェクトを削除時、DestroyWindow()を呼び出す
//		オブジェクトのコピー(例外発生), 代入は禁止
//		titleはクラスの登録にも使うのでそれぞれ独自の名前にしてやる必要がある
//		WM_CREATEなど、最初のCreateWindowで送られるメッセージは届きません
//		(原因：基底クラス内からなのでポリモーフィズムが働かないと考えられる)
//
//	例外(int)：
//		-1 … RegisterClass
//		-2 … sm_this != NULL
//		-3 … m_hWnd == NULL
//		-4 … CWnd( const CWnd & )	// コピーコンストラクタ

#ifndef _CWnd_
#define _CWnd_

#include <windows.h>


class CWnd
{
	static CWnd	*sm_this;

	HINSTANCE	m_hInstance;
	HWND		m_hWnd;


// ウィンドウメッセージ //
#define MSG( method )	virtual LRESULT method( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )	\
			{	return DefWindowProc( hWnd, Msg, wParam, lParam );	}
	virtual bool ExtraMessageProc( HWND, UINT, WPARAM, LPARAM )
	{
		return false;
	}
	LRESULT WMPaint( HWND hWnd, UINT, WPARAM, LPARAM );
	virtual void OnPaint( HWND, HDC ){}
	virtual LRESULT OnCreate( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
	{
		m_cs = *(LPCREATESTRUCT)lParam;
		return DefWindowProc( hWnd, Msg, wParam, lParam );
	}
	MSG( OnDestroy )
	MSG( OnQuit )
	MSG( OnCommand )
	MSG( OnSysCommand )
	MSG( OnActivate )
	MSG( OnActivateApp )
	MSG( OnSetFocus )
	MSG( OnSetText )
	MSG( OnWindowPosChanged )
	MSG( OnWindowPosChanging )
	MSG( OnSize )
	MSG( OnSizing )
	MSG( OnMove )
	MSG( OnMoving )
	MSG( OnEnable )
	MSG( OnDropFiles )
	MSG( OnChar )
	MSG( OnKeyDown )
	MSG( OnKeyUp )
	MSG( OnSysChar )
	MSG( OnSysKeyDown )
	MSG( OnSysKeyUp )
	MSG( OnKillFocus )
	MSG( OnMenuSelect )
	MSG( OnMouseMove )
	MSG( OnMouseWheel )
	MSG( OnLButtonDBLCLK )
	MSG( OnLButtonDown )
	MSG( OnLButtonUp )
	MSG( OnMButtonDBLCLK )
	MSG( OnMButtonDown )
	MSG( OnMButtonUp )
	MSG( OnRButtonDBLCLK )
	MSG( OnRButtonDown )
	MSG( OnRButtonUp )
	MSG( OnTimer )
	MSG( OnDrawItem )
	MSG( OnHScroll )
	MSG( OnVScroll )
	MSG( OnCtlColorBtn )
	MSG( OnCtlColorDlg )
	MSG( OnCtlColorEdit )
	MSG( OnCtlColorListBox )
	MSG( OnCtlColorMsgBox )
	MSG( OnCtlColorScrollBar )
	MSG( OnCtlColorStatic )
	MSG( OnQueryEndSession )

	LRESULT Dispatch( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	static LRESULT CALLBACK WndProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

protected:
	CREATESTRUCT	m_cs;

public:
	CWnd( HINSTANCE hInstance, TCHAR *title, UINT style,
		HWND hWndParent = NULL, HICON hIcon = NULL );
	virtual ~CWnd();
	CWnd( const CWnd & );
	CWnd & operator =( const CWnd & );

	HWND hWnd();
	BOOL Show( int nCmdShow );
	BOOL Move( int X, int Y, int nWidth, int nHeight );
	BOOL Menu( HMENU hMenu );
	void Cursor( LPCTSTR lpCursorName, bool bInstance );

	static WPARAM MessageLoop();
};

typedef CWnd *	PCWnd;


#endif
