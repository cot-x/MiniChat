// CEntryBox
//	入力インターフェースクラス
//
//		RETURNが押されると、親ウィンドウにMsgをSEND。(lParamにテキスト)
//		初期状態はSW_HIDEなので、SW_SHOWを指定してやる必要がある
//		nameはクラスの登録にも使うのでそれぞれ独自の名前にしてやる必要がある
//		オブジェクトのコピー(CWndで例外発生), 代入は禁止

#ifndef _CEntryBox_
#define _CEntryBox_

#include "CWnd.h"
#include <map>


class CEntryBox : public CWnd
{
	HWND				m_hWndParent, m_hEdit;
	UINT				m_Msg;
	COLORREF			m_TextColor, m_TextBkColor;
	HBRUSH				m_hBrush;
	long				m_lLimit;
	static std::map<HWND,WNDPROC>	sm_proc;

	virtual LRESULT OnCreate( HWND hWnd, UINT, WPARAM, LPARAM lParam );
	virtual LRESULT OnSize( HWND, UINT, WPARAM, LPARAM lParam );
	virtual LRESULT OnCtlColorEdit( HWND, UINT, WPARAM wParam, LPARAM );
	virtual LRESULT OnSetFocus( HWND, UINT, WPARAM, LPARAM );
	virtual bool ExtraMessageProc( HWND hWnd, UINT Msg, WPARAM, LPARAM );

	static LRESULT CALLBACK EditProc( HWND, UINT Msg, WPARAM wParam, LPARAM lParam );

public:
	CEntryBox( HINSTANCE hInstance, TCHAR *name, HWND hWndParent, long limit, UINT Msg,
		COLORREF TextColor, COLORREF TextBkColor, HBRUSH hBrush );
	virtual ~CEntryBox();
	CEntryBox & operator =( const CEntryBox & );

	static LONG Height();
};

typedef CEntryBox *	PCEntryBox;


#endif
