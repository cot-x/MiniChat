#include "CSock.h"


#define UM_SOCK		WM_USER+1
#define UM_GOTHOSTENT	WM_USER+2

// private //

bool CSock::ExtraMessageProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	switch( Msg ){
	case UM_SOCK:{
		int	nErrorCode = WSAGETSELECTERROR(lParam);
	
		switch( WSAGETSELECTEVENT(lParam) ){
		case FD_ACCEPT:
			return OnAccept( hWnd, (SOCKET)wParam, nErrorCode );
		case FD_CLOSE:
			return OnClose( (SOCKET)wParam, nErrorCode );
		case FD_CONNECT:
			return OnConnect( (SOCKET)wParam, nErrorCode );
		case FD_OOB:
			return OnOOB( (SOCKET)wParam, nErrorCode );
		case FD_READ:
			return OnRead( (SOCKET)wParam, nErrorCode );
		case FD_WRITE:
			return OnWrite( (SOCKET)wParam, nErrorCode );
		}
			}
	
	case UM_GOTHOSTENT:
		if( (HANDLE)wParam != m_hndlTask )
			return true;
		return GotHostEnt( (LPHOSTENT)m_bufHostEnt, WSAGETASYNCERROR(lParam) );
	}

	return false;
}


// public //

TCHAR	CSock::sm_szErr[255];


CSock::CSock( HINSTANCE hInstance, TCHAR *name, HWND hWndParent )
: CWnd(hInstance, name, WS_DISABLED, hWndParent)
{
}

CSock::~CSock()
{
}

CSock & CSock::operator =( CSock & )
{
	return *this;
}

bool CSock::SelectSocket( SOCKET socket )
{
	if( SOCKET_ERROR == ::WSAAsyncSelect(socket, hWnd(), UM_SOCK,
		FD_ACCEPT | FD_CLOSE | FD_CONNECT | FD_OOB | FD_READ | FD_WRITE) )
		return false;

	return true;
}

bool CSock::GetHostEnt( const char FAR * name )
{
	m_hndlTask = ::WSAAsyncGetHostByName( hWnd(), UM_GOTHOSTENT, name,
		m_bufHostEnt, sizeof(m_bufHostEnt) );
	if( 0 == m_hndlTask )
		return false;

	return true;
}

TCHAR * CSock::ResearchError( int nErrorCode )
{
	switch( nErrorCode ){
	case WSAEACCES:
		::lstrcpy( sm_szErr, TEXT("アクセスが拒否されました。"));
		break;
	case WSAEADDRINUSE:
		::lstrcpy( sm_szErr, TEXT("アドレスはすでに使用されています。"));
		break;
	case WSAEADDRNOTAVAIL:
		::lstrcpy( sm_szErr, TEXT("無効なアドレス。"));
		break;
	case WSAEAFNOSUPPORT:
		::lstrcpy( sm_szErr, TEXT("アドレスファミリはプロトコルファミリでサポートされません。"));
		break;
	case WSAEALREADY:
		::lstrcpy( sm_szErr, TEXT("操作はすでに実行中です。"));
		break;
	case WSAEBADF:
		::lstrcpy( sm_szErr, TEXT("不良ファイル記述子。"));
		break;
	case WSAECANCELLED:
		::lstrcpy( sm_szErr, TEXT("取り消し。"));
		break;
	case WSAECONNABORTED:
		::lstrcpy( sm_szErr, TEXT("ソフトウェアによって接続が拒否されました。"));
		break;
	case WSAECONNREFUSED:
		::lstrcpy( sm_szErr, TEXT("接続が拒否されました。"));
		break;
	case WSAECONNRESET:
		::lstrcpy( sm_szErr, TEXT("接続は相手によってリセットされました。"));
		break;
	case WSAEDESTADDRREQ:
		::lstrcpy( sm_szErr, TEXT("受け側アドレスが必要です。"));
		break;
	case WSAEDISCON:
		::lstrcpy( sm_szErr, TEXT("シャットダウン処理中。"));
		break;
	case WSAEDQUOT:
		::lstrcpy( sm_szErr, TEXT("ディスククォータ。"));
		break;
	case WSAEFAULT:
		::lstrcpy( sm_szErr, TEXT("不良なアドレスです。"));
		break;
	case WSAEHOSTDOWN:
		::lstrcpy( sm_szErr, TEXT("ホストがダウンしています。"));
		break;
	case WSAEHOSTUNREACH:
		::lstrcpy( sm_szErr, TEXT("ホストへのルートがありません。"));
		break;
	case WSAEINPROGRESS:
		::lstrcpy( sm_szErr, TEXT("ブロッキング操作はすでに実行中です。"));
		break;
	case WSAEINTR:
		::lstrcpy( sm_szErr, TEXT("関数呼び出しが中断されました。"));
		break;
	case WSAEINVAL:
		::lstrcpy( sm_szErr, TEXT("無効な引数です。"));
		break;
	case WSAEINVALIDPROCTABLE:
		::lstrcpy( sm_szErr, TEXT("サービスプロバイダからのプロシージャテーブルが無効です。"));
		break;
	case WSAEINVALIDPROVIDER:
		::lstrcpy( sm_szErr, TEXT("無効なサービスプロバイダバージョン番号。"));
		break;
	case WSAEISCONN:
		::lstrcpy( sm_szErr, TEXT("ソケットはすでに接続されています。"));
		break;
	case WSAELOOP:
		::lstrcpy( sm_szErr, TEXT("ループ。"));
		break;
	case WSAEMFILE:
		::lstrcpy( sm_szErr, TEXT("開いているソケットの数が多すぎます。"));
		break;
	case WSAEMSGSIZE:
		::lstrcpy( sm_szErr, TEXT("メッセージが長すぎます。"));
		break;
	case WSAENAMETOOLONG:
		::lstrcpy( sm_szErr, TEXT("名前が長すぎます。"));
		break;
	case WSAENETDOWN:
		::lstrcpy( sm_szErr, TEXT("ネットワークがダウンしています。"));
		break;
	case WSAENETRESET:
		::lstrcpy( sm_szErr, TEXT("ネットワーク接続が破棄されました。"));
		break;
	case WSAENETUNREACH:
		::lstrcpy( sm_szErr, TEXT("ネットワークに到達できません。"));
		break;
	case WSAENOBUFS:
		::lstrcpy( sm_szErr, TEXT("バッファに開き領域がありません。"));
		break;
	case WSAENOMORE:
		::lstrcpy( sm_szErr, TEXT("データはこれ以上ありません。"));
		break;
	case WSAENOPROTOOPT:
		::lstrcpy( sm_szErr, TEXT("不良なプロトコルオプションです。"));
		break;
	case WSAENOTCONN:
		::lstrcpy( sm_szErr, TEXT("ソケットは接続されていません。"));
		break;
	case WSAENOTEMPTY:
		::lstrcpy( sm_szErr, TEXT("ディレクトリが空ではありません。"));
		break;
	case WSAENOTSOCK:
		::lstrcpy( sm_szErr, TEXT("指定されたソケットが無効です。"));
		break;
	case WSAEOPNOTSUPP:
		::lstrcpy( sm_szErr, TEXT("操作がサポートされていません。"));
		break;
	case WSAEPFNOSUPPORT:
		::lstrcpy( sm_szErr, TEXT("プロトコルファミリがサポートされていません。"));
		break;
	case WSAEPROCLIM:
		::lstrcpy( sm_szErr, TEXT("プロセスの数が多すぎます。"));
		break;
	case WSAEPROTONOSUPPORT:
		::lstrcpy( sm_szErr, TEXT("プロトコルがサポートされていません。"));
		break;
	case WSAEPROTOTYPE:
		::lstrcpy( sm_szErr, TEXT("ソケットに対するプロトコルタイプが間違っています。"));
		break;
	case WSAEPROVIDERFAILEDINIT:
		::lstrcpy( sm_szErr, TEXT("サービスプロバイダを初期化できません。"));
		break;
	case WSAEREFUSED:
		::lstrcpy( sm_szErr, TEXT("拒否。"));
		break;
	case WSAEREMOTE:
		::lstrcpy( sm_szErr, TEXT("リモート。"));
		break;
	case WSAESHUTDOWN:
		::lstrcpy( sm_szErr, TEXT("ソケットのシャットダウンの後には通信できません。"));
		break;
	case WSAESOCKTNOSUPPORT:
		::lstrcpy( sm_szErr, TEXT("ソケットタイプがサポートされていません。"));
		break;
	case WSAESTALE:
		::lstrcpy( sm_szErr, TEXT("廃止。"));
		break;
	case WSAETIMEDOUT:
		::lstrcpy( sm_szErr, TEXT("接続がタイムアウトしました。"));
		break;
	case WSAETOOMANYREFS:
		::lstrcpy( sm_szErr, TEXT("参照の数が多すぎます。"));
		break;
	case WSAEUSERS:
		::lstrcpy( sm_szErr, TEXT("ユーザーの数が多すぎます。"));
		break;
	case WSAEWOULDBLOCK:
		::lstrcpy( sm_szErr, TEXT("操作はブロッキングされます。"));
		break;
	case WSAHOST_NOT_FOUND:
		::lstrcpy( sm_szErr, TEXT("ホストが見つかりません。"));
		break;
	case WSANOTINITIALISED:
		::lstrcpy( sm_szErr, TEXT("WSAStartup()がまだ正常に実行されていません。"));
		break;
	case WSANO_DATA:
		::lstrcpy( sm_szErr, TEXT("名前は有効ですが、要求したタイプのデータレコードはありません。"));
		break;
	case WSANO_RECOVERY:
		::lstrcpy( sm_szErr, TEXT("回復不可能なエラー。"));
		break;
	case WSASERVICE_NOT_FOUND:
		::lstrcpy( sm_szErr, TEXT("サービスが見つかりません。"));
		break;
	case WSASYSCALLFAILURE:
		::lstrcpy( sm_szErr, TEXT("システムコールに失敗しました。"));
		break;
	case WSASYSNOTREADY:
		::lstrcpy( sm_szErr, TEXT("ネットワークサブシステムを利用できません。"));
		break;
	case WSATRY_AGAIN:
		::lstrcpy( sm_szErr, TEXT("ホストが見つからないかサーバーの異常です。"));
		break;
	case WSATYPE_NOT_FOUND:
		::lstrcpy( sm_szErr, TEXT("タイプが見つかりません。"));
		break;
	case WSAVERNOTSUPPORTED:
		::lstrcpy( sm_szErr, TEXT("WINSOCK.DLLのバージョンが範囲外です。"));
		break;
	case WSA_E_CANCELLED:
		::lstrcpy( sm_szErr, TEXT("検索が取り消されました。"));
		break;
	case WSA_E_NO_MORE:
		::lstrcpy( sm_szErr, TEXT("データはこれ以上ありません。"));
		break;
	default:
		::lstrcpy( sm_szErr, TEXT("不明なエラーコードです。"));
	}

	return sm_szErr;
}
