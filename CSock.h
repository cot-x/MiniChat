// CSock
//	非同期モードソケット処理ウィンドウクラス
//
//		実行前にWSAStartup()でイニシャライズしておく必要があります
//		WinSock2のため、WS2_32.LIBのリンクが必要です
//		オブジェクトのコピー(CWndで例外発生), 代入は禁止

#ifndef _CSock_
#define _CSock_

// winsock2.hを必ずwindows.hより先にインクルードする
//	(winsock2.hの中でwindows.hはインクルードされる)
#include <winsock2.h>
#include "CWnd.h"


class CSock : public CWnd
{
	HANDLE	m_hndlTask;
	char	m_bufHostEnt[MAXGETHOSTSTRUCT];


	virtual bool ExtraMessageProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

	virtual bool OnAccept( HWND, SOCKET, int ){	return false;	}
	virtual bool OnClose( SOCKET, int ){	return false;	}
	virtual bool OnConnect( SOCKET, int ){	return false;	}
	virtual bool OnOOB( SOCKET, int ){	return false;	}
	virtual bool OnRead( SOCKET, int ){	return false;	}
	virtual bool OnWrite( SOCKET, int ){	return false;	}
	virtual bool GotHostEnt( LPHOSTENT, WORD ){	return false;	}

public:
	static TCHAR	sm_szErr[];


	CSock( HINSTANCE hInstance, TCHAR *name, HWND hWndParent );
	virtual ~CSock();
	CSock & operator =( CSock & );

	bool SelectSocket( SOCKET socket );
	bool GetHostEnt( const char FAR * name );

	static TCHAR * ResearchError( int nErrorCode );
};

typedef CSock *	PCSock;


#endif
