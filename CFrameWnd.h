// CFrameWnd
//	フレームウィンドウ
//
//		初期状態はSW_HIDEなので、SW_SHOWを指定してやる必要がある
//		titleはクラスの登録にも使うのでそれぞれ独自の名前にしてやる必要がある
//		オブジェクトのコピー(CWndで例外発生), 代入は禁止

#ifndef _CFrameWnd_
#define _CFrameWnd_

#include "CMiniChat.h"	// winsock2.hをインクルードするため、一番最初にインクルード
#include "CWnd.h"
#include "CMsgEdit.h"
#include "CEntryBox.h"


class CFrameWnd : public CWnd
{

	int		m_round, m_YCaption, m_XCaption, m_nWidth, m_nHeight;
	bool		m_bClosing, m_bMoving;
	POINT		m_pos;

	PCMiniChat	m_pcMiniChat;
	PCMsgEdit	m_pcMsgEdit;
	PCEntryBox	m_pcEntryBox;
	bool		m_bServer;
	HBITMAP		m_hBitmap;
	BITMAP		m_bitmap;
	struct SNetSelect
	{
		TCHAR		*szAddr;
		TCHAR		*szHN;
		u_short		nPort;
	} m_sNetSelect;


	virtual bool ExtraMessageProc( HWND, UINT Msg, WPARAM, LPARAM lParam );
	virtual LRESULT OnSetFocus( HWND, UINT, WPARAM, LPARAM );
	static BOOL CALLBACK DialogProc( HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam );

// CFrameWnd内メッセージ処理
	void OnFrameCreate( HWND hWnd, UINT, WPARAM, LPARAM lParam );
	void OnFrameDestroy( HWND, UINT, WPARAM, LPARAM );
	void OnFrameSize( HWND, RECT *pRect, int );
	void OnFramePaint( HWND hWnd, HDC hdc );

// メッセージ処理
	virtual void OnPaint( HWND hWnd, HDC hdc );
	virtual LRESULT OnCreate( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnDestroy( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnSize( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnLButtonDown( HWND hWnd, UINT, WPARAM, LPARAM lParam );
	virtual LRESULT OnLButtonUp( HWND hWnd, UINT, WPARAM, LPARAM lParam );
	virtual LRESULT OnMouseMove( HWND hWnd, UINT, WPARAM, LPARAM );
	virtual LRESULT OnSetText( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

public:
	CFrameWnd( HINSTANCE hInstance, TCHAR *title, HICON hIcon = NULL );
	virtual ~CFrameWnd();
	CFrameWnd & operator =( const CFrameWnd & );
	void SelectNet( HINSTANCE hInstance );
};

typedef CFrameWnd *	PCFrameWnd;


#endif
