#include "CMiniChat.h"

#define WSAVersion	MAKEWORD(1,1)
#define BUFSIZE		1024
#define MAXCLIENT	8


// private //

LRESULT CMiniChat::OnCreate( HWND, UINT, WPARAM, LPARAM lParam )
{
	int	nRet;


	m_hInstance = ((LPCREATESTRUCT)lParam)->hInstance;

	nRet = ::WSAStartup( WSAVersion, &m_wsaData );
	if( 0 != nRet ){
		::MessageBox( m_hWndParent, TEXT("WSAStartup()に失敗しました。"),
			NULL, MB_ICONERROR );
		::DestroyWindow( m_hWndParent );
		return -1;
	}

	if( WSAVersion != m_wsaData.wVersion ){
		::MessageBox( m_hWndParent, TEXT("要求されたWinSockバージョンが利用できません。"),
			NULL, MB_ICONERROR );
		::WSACleanup();
		::DestroyWindow( m_hWndParent );
		return -1;
	}

	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("Init ") );
	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)m_wsaData.szDescription );
	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT(" ... OK") );
	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );

	m_socket = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( INVALID_SOCKET == m_socket ){
		::MessageBox( m_hWndParent, TEXT("ソケットの作成に失敗しました。"),
			NULL, MB_ICONERROR );
		::DestroyWindow( m_hWndParent );
		return -1;
	}

	if( !SelectSocket(m_socket) ){
		::MessageBox( m_hWndParent, ResearchError(::WSAGetLastError()),
			NULL, MB_ICONERROR );
		::DestroyWindow( m_hWndParent );
		return -1;
	}

	if( m_bServer ){
		SOCKADDR_IN	saServer;
	
		saServer.sin_family		= AF_INET;
		saServer.sin_port		= ::htons(m_nPort);
		saServer.sin_addr.s_addr	= INADDR_ANY;
		if( ::bind(m_socket, (LPSOCKADDR)&saServer, sizeof(saServer)) ){
			::MessageBox( m_hWndParent, ResearchError(::WSAGetLastError()),
				NULL, MB_ICONERROR );
			::DestroyWindow( m_hWndParent );
			return -1;
		}
	
		if( ::listen(m_socket, SOMAXCONN) ){
			::MessageBox( m_hWndParent, ResearchError(::WSAGetLastError()),
				NULL, MB_ICONERROR );
			::DestroyWindow( m_hWndParent );
			return -1;
		}
	
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("Server Start!\r\n") );
		PlaySound();
	}else{
		if( !GetHostEnt(m_szAddr) ){
			::MessageBox( m_hWndParent, ResearchError(::WSAGetLastError()),
				NULL, MB_ICONERROR );
			::DestroyWindow( m_hWndParent );
			return -1;
		}
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("Serching Host ... ") );
	}

	return 0;
}

LRESULT CMiniChat::OnDestroy( HWND, UINT, WPARAM, LPARAM )
{
	if( m_socket ){
		if( !m_bServer && m_bConnected ){
			if( ::shutdown(m_socket, SD_BOTH) == SOCKET_ERROR )
				::MessageBox( m_hWndParent, ResearchError(::WSAGetLastError()),
					NULL, MB_ICONERROR );
		}
		if( m_bServer ){
			std::map<SOCKET,std::string>::iterator p;
		
			p = m_send.begin();
			while( p != m_send.end() ){
				::shutdown( p->first, SD_BOTH );
				::closesocket( p->first );
				DeleteData( p->first );
				p = m_send.begin();
			}
		}
		if( ::closesocket(m_socket) == SOCKET_ERROR )
			::MessageBox( m_hWndParent, ResearchError(::WSAGetLastError()),
				NULL, MB_ICONERROR );
	}

	::WSACleanup();

	return 0;
}

bool CMiniChat::OnAccept( HWND, SOCKET, int nErrorCode )
{
	SOCKET		ClientSocket;
	SOCKADDR_IN	saClient;
	int		nLen;


	if( 0 != nErrorCode ){
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)ResearchError(nErrorCode) );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		return true;
	}

	nLen = sizeof(saClient);
	ClientSocket = ::accept( m_socket, (LPSOCKADDR)&saClient, &nLen );
	if( SOCKET_ERROR == ClientSocket && ::WSAGetLastError() == WSAEWOULDBLOCK ){
		::SendMessage( m_hWndParent, m_Msg, 0,
			(LPARAM)ResearchError(::WSAGetLastError()) );
		return true;
	}

	if( MAXCLIENT <= m_nClient ){
		if( ::shutdown(ClientSocket, SD_BOTH) == SOCKET_ERROR ){
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)ResearchError(::WSAGetLastError()) );
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		}
		if( ::closesocket(ClientSocket) == SOCKET_ERROR ){
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)ResearchError(::WSAGetLastError()) );
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		}
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("定員オーバーの為") );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)::inet_ntoa(saClient.sin_addr) );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("を切断しました。\r\n") );
		SendChat( "定員オーバーの為" );
		SendChat( ::inet_ntoa(saClient.sin_addr) );
		SendChat( "を切断しました。\r\n" );
		PlaySound();
		return true;
	}

	if( !SelectSocket(ClientSocket) ){	// 確実にするため
		::SendMessage( m_hWndParent, m_Msg, 0,
			(LPARAM)ResearchError(::WSAGetLastError()) );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		if( ::shutdown(ClientSocket, SD_BOTH) == SOCKET_ERROR ){
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)ResearchError(::WSAGetLastError()) );
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		}
		if( ::closesocket(ClientSocket) == SOCKET_ERROR ){
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)ResearchError(::WSAGetLastError()) );
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		}
	}
	CreateData( ClientSocket, ::inet_ntoa(saClient.sin_addr) );
	m_nClient++;

	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("Accept: ") );
	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)::inet_ntoa(saClient.sin_addr) );
	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );

	return true;
}

bool CMiniChat::OnClose( SOCKET socket, int nErrorCode )
{
	std::map<SOCKET,std::string>::iterator	p;
	std::string				str_msg;


	if( !m_bServer ){
		if( 0 != nErrorCode ){
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)ResearchError(nErrorCode) );
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		}else{
			::MessageBox( m_hWndParent, TEXT("サーバーとの通信がきれました。\r\n"),
				NULL, MB_ICONERROR );
		}
		::DestroyWindow( m_hWndParent );
		return true;
	}

	p = m_HN.find(socket);
	if( m_HN.end() != p ){
		str_msg += p->second;
		m_nClient--;
	}
	str_msg += ": ";

	if( 0 != nErrorCode ){
		str_msg += ResearchError( nErrorCode );
		str_msg += "\r\n";
	}else{
		str_msg += "退室しました。\r\n";
	}

	SendChat( str_msg.c_str() );
	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)str_msg.c_str() );
	PlaySound();

	if( ::closesocket(socket) == SOCKET_ERROR ){
		::SendMessage( m_hWndParent, m_Msg, 0,
			(LPARAM)ResearchError(::WSAGetLastError()) );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
	}
	DeleteData( socket );

	return true;
}

bool CMiniChat::OnConnect( SOCKET socket, int nErrorCode )
{
	if( 0 != nErrorCode ){
		::MessageBox( m_hWndParent, ResearchError(nErrorCode), NULL, MB_ICONERROR );
		::DestroyWindow( m_hWndParent );
		return true;
	}

	CreateData( socket, m_szAddr );
	m_bConnected = true;
	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("Connecting ... OK\r\n") );

	SendChat("/HN ");
	SendChat( m_szHN );
	SendChat("\r\n");

	return true;
}

bool CMiniChat::OnRead( SOCKET socket, int nErrorCode )
{
	int					nBytesRecv;
	unsigned int				nPos;
	char					buf[BUFSIZE];
	std::map<SOCKET,std::string>::iterator	p;


	if( 0 != nErrorCode ){
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)ResearchError(nErrorCode) );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		return true;
	}

	nBytesRecv = ::recv( socket, buf, BUFSIZE-1, 0 );
	if( SOCKET_ERROR == nBytesRecv ){
		if( ::WSAGetLastError() != WSAEWOULDBLOCK ){
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)ResearchError(::WSAGetLastError()) );
		}else{
			return true;
		}
	}else if( SOCKET_ERROR != nBytesRecv ){
		buf[nBytesRecv] = '\0';
	}

	p = m_recv.find(socket);
	if( m_recv.end() != p )
		p->second += buf;

	nPos = p->second.find("\n");
	while( std::string::npos != nPos ){
		std::string	str;
		bool		bSend = true;
	
		str = p->second.substr(0, nPos+1);
		p->second = p->second.substr(nPos+1);
	
		if( m_bServer ){
			std::string	send_str;
		
			p = m_HN.find(socket);
			if( m_HN.end() == p )
				send_str = "> " + str;
			else
				send_str = (m_HN.find(socket))->second + "> " + str;
		
			if( str[0] == '/' ){
				bSend = false;
				RecvCommand( socket, &str, &send_str );
			}
		
			if( bSend ){
				SendChat( send_str.c_str() );
				::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)send_str.c_str() );
			}
		}else{
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)str.c_str() );
		}
	
		if( bSend )
			PlaySound();
	
		nPos = p->second.find("\n");
	}

	return true;
}

bool CMiniChat::OnWrite( SOCKET socket, int nErrorCode )
{
	std::map<SOCKET,std::string>::iterator	p;
	int					nBytesSent, nLen;


	if( 0 != nErrorCode ){
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)ResearchError(nErrorCode) );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		return true;
	}

	p = m_send.find(socket);
	if( m_send.end() != p ){
		nLen = ::lstrlen( p->second.c_str() );
		nBytesSent = ::send( p->first, p->second.c_str(), nLen, 0 );
		if( SOCKET_ERROR == nBytesSent && ::WSAGetLastError() != WSAEWOULDBLOCK ){
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)ResearchError(::WSAGetLastError()) );
		}else if( SOCKET_ERROR != nBytesSent ){
			p->second = p->second.substr(nBytesSent);
		}
	}

	return true;
}

bool CMiniChat::GotHostEnt( LPHOSTENT lpHostEnt, WORD nErrorCode )
{
	SOCKADDR_IN	saClient;


	if( 0 != nErrorCode ){
		::MessageBox( m_hWndParent, ResearchError(nErrorCode), NULL, MB_ICONERROR );
		::DestroyWindow( m_hWndParent );
		return true;
	}

	saClient.sin_family	= AF_INET;
	saClient.sin_port	= htons(m_nPort);
	saClient.sin_addr	= *(LPIN_ADDR)*lpHostEnt->h_addr_list;

	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("OK\r\nLet's connect to ") );
	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)::inet_ntoa(saClient.sin_addr) );
	::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("!\r\n") );

	if( ::connect(m_socket, (LPSOCKADDR)&saClient, sizeof(saClient)) == SOCKET_ERROR
		&& ::WSAGetLastError() != WSAEWOULDBLOCK ){
		::MessageBox( m_hWndParent,
			ResearchError(::WSAGetLastError()), NULL, MB_ICONERROR );
		::DestroyWindow( m_hWndParent );
		return true;
	}

	return true;
}

void CMiniChat::CreateData( SOCKET socket, char *szHN )
{
	m_send.insert(std::pair<SOCKET,std::string>(socket, ""));
	m_recv.insert(std::pair<SOCKET,std::string>(socket, ""));
	m_HN.insert(std::pair<SOCKET,std::string>(socket, szHN));
}

bool CMiniChat::DeleteData( SOCKET socket )
{
	std::map<SOCKET,std::string>::iterator p;
	bool	bRet = true;


	p = m_send.find(socket);
	if( m_send.end() == p )
		bRet = false;
	m_send.erase(p);

	p = m_recv.find(socket);
	if( m_recv.end() == p )
		bRet = false;
	m_recv.erase(p);

	p = m_HN.find(socket);
	if( m_HN.end() == p )
		bRet = false;
	m_HN.erase(p);

	return bRet;
}

bool CMiniChat::Send( SOCKET socket, const char *szText )
{
	std::map<SOCKET,std::string>::iterator	p;
	int					nBytesSent, nLen;


	p = m_send.find(socket);
	if( m_send.end() == p )
		return false;

	p->second += szText;

	nLen = ::lstrlen( p->second.c_str() );
	nBytesSent = ::send( p->first, p->second.c_str(), nLen, 0 );
	if( SOCKET_ERROR == nBytesSent && ::WSAGetLastError() != WSAEWOULDBLOCK ){
		::SendMessage( m_hWndParent, m_Msg, 0,
			(LPARAM)ResearchError(::WSAGetLastError()) );
	}else if( SOCKET_ERROR != nBytesSent ){
		p->second = p->second.substr(nBytesSent);
	}

	return true;
}

void CMiniChat::RecvCommand( SOCKET socket, std::string *recv_str, std::string *send_str )
{
	std::map<SOCKET,std::string>::iterator	p;
	unsigned int				nPos;


	nPos = recv_str->find("HN ");
	if( std::string::npos != nPos && nPos == 1 ){
		std::string	str_newHN;
		bool		bChange = true;
	
		SendChat( send_str->c_str() );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)send_str->c_str() );
	
		str_newHN = recv_str->substr(nPos+3);
		str_newHN = str_newHN.substr( 0, str_newHN.find("\r") );
		if( CheckHN(str_newHN.c_str()) ){
			SendChat( "すでに同じ名前が使われているため使用できません。\r\n" );
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)"すでに同じ名前が使われて"
								"いるため使用できません。\r\n" );
			bChange = false;
		}
		if( ::lstrcmp(str_newHN.c_str(), m_szHN) == 0 ){
			SendChat( "すでに同じ名前が使われているため使用できません。\r\n" );
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)"すでに同じ名前が使われて"
								"いるため使用できません。\r\n" );
			bChange = false;
		}
		
		p = m_HN.find(socket);
		if( bChange && m_HN.end() != p ){
			p->second = str_newHN;
			SendChat( "HNを変更しました\r\n" );
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)"HNを変更しました。\r\n" );
		}
	
		PlaySound();
	
		return;
	}
	nPos = recv_str->find("member");
	if( std::string::npos != nPos && nPos == 1 ){
		std::string	str_member;
	
		str_member = "/member\r\n";
	
		str_member += m_szHN;
		str_member += "(Server)";
		str_member += "\r\n";
	
		p = m_HN.begin();
		while( m_HN.end() != p ){
			str_member += p->second;
			str_member += "\r\n";
			p++;
		}
		Send( socket, str_member.c_str() );
	
		return;
	}
	nPos = recv_str->find("whisper ");
	if( m_bServer && std::string::npos != nPos && nPos == 1 ){
		std::string	str_HN, str_text;
		SOCKET		whisock = NULL;
		const char	*szHN;
	
		Send( socket, recv_str->c_str() );
	
		str_HN = str_text = recv_str->substr(nPos+8);
		nPos = str_HN.find(" ");
		if( std::string::npos == nPos ){
			Send( socket, "コマンドエラー: /whisper HN メッセージ\r\n" );
			return;
		}
		str_HN = str_HN.substr(0,nPos);
		str_text = str_text.substr(nPos+1);
	
		p = m_HN.begin();
		while( m_HN.end() != p ){
			if( ::lstrcmp(str_HN.c_str(),
					p->second.c_str()) == 0 ){
				whisock = p->first;
				break;
			}
			p++;
		}
	
		p = m_HN.find(socket);
		if( m_HN.end() != p )
			szHN = p->second.c_str();
		else
			szHN = NULL;
	
		if( !whisock && ::lstrcmp(str_HN.c_str(), m_szHN) == 0 ){
			::SendMessage( m_hWndParent,
				m_Msg, 0,
				(LPARAM)TEXT("Whisper "
				"from ") );
			::SendMessage( m_hWndParent,
				m_Msg, 0,
				(LPARAM)szHN );
			::SendMessage( m_hWndParent,
				m_Msg, 0,
				(LPARAM)TEXT(": ") );
			::SendMessage( m_hWndParent,
				m_Msg, 0,
				(LPARAM)str_text.c_str() );
			PlaySound();
			Send( socket, "送信しました。\r\n" );
			return;
		}
	
		if( whisock ){
			Send( whisock, "Whisper from " );
			Send( whisock, szHN );
			Send( whisock, ": " );
			Send( whisock, str_text.c_str() );
			Send( socket, "送信しました。\r\n" );
		}else{
			Send( socket, "Unknown HN.\r\n" );
		}
		
		return;
	}
}


// public //

CMiniChat::CMiniChat( HINSTANCE hInstance, TCHAR *name, HWND hWndParent, UINT Msg,
	TCHAR *szHN, bool bServer, TCHAR *szAddr, u_short nPort  )
: CSock( hInstance, name, hWndParent ),
	m_hWndParent(hWndParent), m_Msg(Msg),
	m_szHN(szHN), m_szAddr(szAddr), m_bServer(bServer), m_nPort(nPort),
	m_socket(NULL), m_bConnected(false), m_nClient(0), m_bSound(true)
{
	OnCreate( hWnd(), WM_CREATE, 0, (LPARAM)&m_cs );
}

CMiniChat::~CMiniChat()
{
}

CMiniChat & CMiniChat::operator =( const CMiniChat & )
{
	return *this;
}

bool CMiniChat::SendChat( const char *szText )
{
	std::map<SOCKET,std::string>::iterator	p;
	int					nBytesSent, nLen;


	p = m_send.begin();
	if( m_send.end() == p )
		return false;

	while( m_send.end() != p ){
		p->second += szText;
	
		nLen = ::lstrlen( p->second.c_str() );
		nBytesSent = ::send( p->first, p->second.c_str(), nLen, 0 );
		if( SOCKET_ERROR == nBytesSent && ::WSAGetLastError() != WSAEWOULDBLOCK ){
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)ResearchError(::WSAGetLastError()) );
		}else if( SOCKET_ERROR != nBytesSent ){
			p->second = p->second.substr(nBytesSent);
		}
	
		p++;
	}

	return true;
}

BOOL CMiniChat::PlaySound()
{
	if( m_bSound )
		return ::PlaySound( TEXT("TEXT_WAV"), m_hInstance,
			SND_ASYNC | SND_NODEFAULT | SND_NOSTOP | SND_NOWAIT | SND_RESOURCE );

	return FALSE;
}

void CMiniChat::ChangeHN( TCHAR *szHN )
{
	m_szHN = szHN;
}

bool CMiniChat::CheckHN( const char *szHN )
{
	std::map<SOCKET,std::string>::iterator	p;


	p = m_HN.begin();
	while( m_HN.end() != p ){
		if( ::lstrcmp(szHN, p->second.c_str()) == 0 )
			return true;
		
		p++;
	}

	return false;
}

bool CMiniChat::CheckCommand( TCHAR *szCommand )
{
	unsigned int	nPos;
	std::string	str = szCommand;


	if( str[0] != '/' )
		return false;

	nPos = str.find("sound on");
	if( std::string::npos != nPos && nPos == 1 ){
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)szCommand );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		m_bSound = true;
		return true;
	}
	nPos = str.find("sound off");
	if( std::string::npos != nPos && nPos == 1 ){
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)szCommand );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
		m_bSound = false;
		return true;
	}
	nPos = str.find("member");
	if( m_bServer && std::string::npos != nPos && nPos == 1 ){
		std::map<SOCKET,std::string>::iterator	p;
	
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)szCommand );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
	
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)m_szHN );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("(Server)") );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
	
		p = m_HN.begin();
		while( m_HN.end() != p ){
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)p->second.c_str() );
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
			p++;
		}
		return true;
	}
	nPos = str.find("whisper ");
	if( m_bServer && std::string::npos != nPos && nPos == 1 ){
		std::map<SOCKET,std::string>::iterator	p;
		std::string				str_HN, str_text;
		SOCKET					socket = NULL;
	
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)szCommand );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
	
		str_HN = str_text = str.substr(nPos+8);
		nPos = str_HN.find(" ");
		if( std::string::npos == nPos ){
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)TEXT("コマンドエラー: /whisper HN メッセージ\r\n") );
			return true;
		}
		str_HN = str_HN.substr(0,nPos);
		str_text = str_text.substr(nPos+1);
		p = m_HN.begin();
		while( m_HN.end() != p ){
			nPos = p->second.find(str_HN);
			if( std::string::npos != nPos ){
				socket = p->first;
				break;
			}
			p++;
		}
	
		if( socket ){
			Send( socket, "Whisper from " );
			Send( socket, m_szHN );
			Send( socket, ": " );
			Send( socket, str_text.c_str() );
			Send( socket, "\r\n" );
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)TEXT("送信しました。\r\n") );
		}else{
			::SendMessage( m_hWndParent, m_Msg, 0,
				(LPARAM)TEXT("Unknown HN.\r\n") );
		}
		return true;
	}
	nPos = str.find("discon ");
	if( m_bServer && std::string::npos != nPos && nPos == 1 ){
		std::map<SOCKET,std::string>::iterator	p;
		std::string				str_HN;
		bool					bDiscon = false;
	
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)szCommand );
		::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("\r\n") );
	
		str_HN = str.substr(nPos+7);
		p = m_HN.begin();
		while( m_HN.end() != p ){
			if( ::lstrcmp(str_HN.c_str(), p->second.c_str()) == 0 ){
				if( ::shutdown(p->first, SD_BOTH) == SOCKET_ERROR ){
					::SendMessage( m_hWndParent, m_Msg, 0,
						(LPARAM)ResearchError(::WSAGetLastError()) );
					::SendMessage( m_hWndParent, m_Msg, 0,
						(LPARAM)TEXT("\r\n") );
				}
				if( ::closesocket(p->first) == SOCKET_ERROR ){
					::SendMessage( m_hWndParent, m_Msg, 0,
						(LPARAM)ResearchError(::WSAGetLastError()) );
					::SendMessage( m_hWndParent, m_Msg, 0,
						(LPARAM)TEXT("\r\n") );
				}
				::closesocket( p->first );
				DeleteData( p->first );
				::SendMessage( m_hWndParent, m_Msg, 0,
					(LPARAM)str_HN.c_str() );
				::SendMessage( m_hWndParent, m_Msg, 0,
					(LPARAM)TEXT("を切断しました。\r\n") );
				SendChat( str_HN.c_str() );
				SendChat( "を切断しました。\r\n" );
				m_nClient--;
				bDiscon = true;
				break;
			}
			p++;
		}
		if( !bDiscon )
			::SendMessage( m_hWndParent, m_Msg, 0, (LPARAM)TEXT("Unknown HN.\r\n") );
		return true;
	}

	return false;
}
