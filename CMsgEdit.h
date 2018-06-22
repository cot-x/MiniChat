// CMsgEdit
//	メッセージ処理機能付きエディットコントロールクラス
//
//		入りきらなかったら、最上位行のテキストを削除して入力します
//		初期状態はSW_HIDEなので、SW_SHOWを指定してやる必要がある
//		nameはクラスの登録にも使うのでそれぞれ独自の名前にしてやる必要がある
//		オブジェクトのコピー(CWndで例外発生), 代入は禁止

#ifndef _CMsgEdit_
#define _CMsgEdit_

#include "CWnd.h"


class CMsgEdit : public CWnd
{
	long			m_lLen;
	HWND			m_hEdit;
	COLORREF		m_TextColor, m_TextBkColor;
	HBRUSH			m_hBrush;

	virtual LRESULT OnCreate( HWND hWnd, UINT, WPARAM, LPARAM lParam );
	virtual LRESULT OnSize( HWND, UINT, WPARAM, LPARAM lParam );
	virtual LRESULT OnCtlColorStatic( HWND, UINT, WPARAM wParam, LPARAM );
	virtual LRESULT OnSetFocus( HWND, UINT, WPARAM, LPARAM );

public:
	CMsgEdit( HINSTANCE hInstance, TCHAR *name, HWND hWndParent,
		COLORREF TextColor, COLORREF TextBkColor, HBRUSH hBrush );
	virtual ~CMsgEdit();
	CMsgEdit & operator =( const CMsgEdit & );

	bool SetMsg( TCHAR *szText );
};

typedef CMsgEdit *	PCMsgEdit;


#endif
