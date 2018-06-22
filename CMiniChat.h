// CMiniChat
//	MiniChatネットワーク処理クラス
//
//		チャットに出力する文字列をhWndParentにMsgのlParamでSendします
//		WinSock2のため、WS2_32.LIBのリンクが必要です
//		オブジェクトのコピー(CWndで例外発生), 代入は禁止

#ifndef _CMiniChat_
#define _CMiniChat_

#include "CSock.h"
#include <map>
#include <string>


class CMiniChat : public CSock
{
	int				m_nClient;
	HWND				m_hWndParent;
	UINT				m_Msg;
	TCHAR				*m_szHN, *m_szAddr;
	u_short				m_nPort;
	bool				m_bServer, m_bConnected, m_bSound;
	WSADATA				m_wsaData;
	SOCKET				m_socket;
	std::map<SOCKET,std::string>	m_send, m_recv, m_HN;
	HINSTANCE			m_hInstance;


	virtual LRESULT OnCreate( HWND, UINT, WPARAM, LPARAM lParam );
	virtual LRESULT OnDestroy( HWND, UINT, WPARAM, LPARAM );

	virtual bool OnAccept( HWND, SOCKET, int nErrorCode );
	virtual bool OnClose( SOCKET socket, int nErrorCode );
	virtual bool OnConnect( SOCKET, int nErrorCode );
	virtual bool OnRead( SOCKET socket, int nErrorCode );
	virtual bool OnWrite( SOCKET socket, int nErrorCode );
	virtual bool GotHostEnt( LPHOSTENT lpHostEnt, WORD nErrorCode );

	void CreateData( SOCKET socket, char *szHN );
	bool DeleteData( SOCKET socket );
	bool Send( SOCKET socket, const char *szText );
	void RecvCommand( SOCKET socket, std::string *recv_str, std::string *send_str );

public:
	CMiniChat( HINSTANCE hInstance, TCHAR *name, HWND hWndParent, UINT Msg,
		TCHAR *szHN, bool bServer, TCHAR *szAddr, u_short nPort );
	virtual ~CMiniChat();
	CMiniChat & operator =( const CMiniChat & );

	bool SendChat( const char *szText );
	BOOL PlaySound();
	void ChangeHN( TCHAR *szHN );
	bool CheckHN( const char *szHN );
	bool CheckCommand( TCHAR *szCommand );
};

typedef CMiniChat *	PCMiniChat;


#endif
