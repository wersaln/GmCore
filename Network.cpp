#include "Network.h"

#pragma comment(lib,"ws2_32.lib")
Network::Network(int i_port)
{
	port = i_port;
	connection.ID = WM_USER + 1;
	connection.socket = NULL;
}
Network::~Network()
{
	closesocket(connection.socket);
	WSACleanup();
}
void Network::ClientInit( HWND hWnd, char *ip)
{

	// Set up Winsock
	WSADATA WsaDat;
	int nResult=WSAStartup(MAKEWORD(2,2),&WsaDat);
	if(nResult!=0)
	{
		MessageBox(hWnd, L"Winsock initialization failed", L"Critical Error",	MB_ICONERROR);
		SendMessage(hWnd,WM_DESTROY,NULL,NULL);
		return;
	}
	
	connection.socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(connection.socket == INVALID_SOCKET)
	{
		MessageBox(hWnd,
			L"Socket creation failed",
			L"Critical Error",
			MB_ICONERROR);
		SendMessage(hWnd,WM_DESTROY,NULL,NULL);
		return;
	}

	nResult=WSAAsyncSelect(connection.socket, hWnd, connection.ID, (FD_CLOSE|FD_READ));
	if(nResult)
	{
		MessageBox(hWnd,
			L"WSAAsyncSelect failed",
			L"Critical Error",
			MB_ICONERROR);
		SendMessage(hWnd,WM_DESTROY,NULL,NULL);
		return;
	}

	// Resolve IP address for hostname
	struct hostent *host;
	if((host=gethostbyname(ip))==NULL)
	{
		MessageBox(hWnd,
			L"Unable to resolve host name",
			L"Critical Error",
			MB_ICONERROR);
		SendMessage(hWnd,WM_DESTROY,NULL,NULL);
		return;
	}

	// Set up our socket address structure
	SOCKADDR_IN SockAddr;
	SockAddr.sin_port=htons(port);
	SockAddr.sin_family=AF_INET;
	//SockAddr.sin_addr = *((LPIN_ADDR)*hostEnt->h_addr_list);
	SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);
	int res = connect(connection.socket, (LPSOCKADDR)(&SockAddr), sizeof(SockAddr));
	if (res == SOCKET_ERROR)
	{
		MessageBox( NULL, L"Could not find tiger.x", L"Meshes.exe", MB_OK );
		return;
	}
}
void Network::MsgHandler( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_CREATE)
	{
		WSADATA WsaDat;
		int nResult=WSAStartup(MAKEWORD(2,2),&WsaDat);
		if(nResult!=0)
		{
			MessageBox(hWnd, L"Winsock initialization failed", L"Critical Error",	MB_ICONERROR);
			SendMessage(hWnd,WM_DESTROY,NULL,NULL);
			return;
		}
	
		connection.socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(connection.socket == INVALID_SOCKET)
		{
			MessageBox(hWnd,
				L"Socket creation failed",
				L"Critical Error",
				MB_ICONERROR);
			SendMessage(hWnd,WM_DESTROY,NULL,NULL);
			return;
		}

		nResult=WSAAsyncSelect(connection.socket, hWnd, connection.ID, (FD_CLOSE|FD_READ));
		if(nResult)
		{
			MessageBox(hWnd,
				L"WSAAsyncSelect failed",
				L"Critical Error",
				MB_ICONERROR);
			SendMessage(hWnd,WM_DESTROY,NULL,NULL);
			return;
		}

		// Resolve IP address for hostname
		struct hostent *host;
		if((host=gethostbyname("localhost"))==NULL)
		{
			MessageBox(hWnd,
				L"Unable to resolve host name",
				L"Critical Error",
				MB_ICONERROR);
			SendMessage(hWnd,WM_DESTROY,NULL,NULL);
			return;
		}

		// Set up our socket address structure
		SOCKADDR_IN SockAddr;
		SockAddr.sin_port=htons(port);
		SockAddr.sin_family=AF_INET;
		//SockAddr.sin_addr = *((LPIN_ADDR)*hostEnt->h_addr_list);
		SockAddr.sin_addr.s_addr=*((unsigned long*)host->h_addr);
		int res = connect(connection.socket, (LPSOCKADDR)(&SockAddr), sizeof(SockAddr));
		if (res == SOCKET_ERROR)
		{
			MessageBox( NULL, L"Could not find tiger.x", L"Meshes.exe", MB_OK );
			return;
		}
	}
	if(uMsg == connection.ID)
	{
	    if(WSAGETSELECTERROR(lParam))
		{	
			MessageBox(hWnd,
				L"Connection to server failed",
				L"Error",
				MB_OK|MB_ICONERROR);
			SendMessage(hWnd,WM_DESTROY,NULL,NULL);
			return;
		}
		switch(WSAGETSELECTEVENT(lParam))
		{
			case FD_READ:
			{
				char szRecvBuff[1024], szSendBuff[1024];
				ZeroMemory(szRecvBuff, sizeof(szRecvBuff));
                ZeroMemory(szSendBuff, sizeof(szSendBuff));
				int ret = recv(wParam, szRecvBuff, 1024, 0);
				if (ret == 0)
			       break;
				else if (ret == SOCKET_ERROR)
				{
					MessageBox(0, L"Recive data filed",
						L"Error", 0);
					break;
				}
				szRecvBuff[ret] = '\0';

				strcpy(szSendBuff, "Command get OK");

				ret = send(wParam, szSendBuff,
					sizeof(szSendBuff), 0);
				break;
			}
			case FD_WRITE:
			{
			    //Ready to send data
			    break;
			}
			case FD_CLOSE:
			{
				MessageBox(hWnd,
					L"Server closed connection",
					L"Connection closed!",
					MB_ICONINFORMATION|MB_OK);
				closesocket(connection.socket);
				SendMessage(hWnd,WM_DESTROY,NULL,NULL);
				break;
			}
			
		}
	} 
}
bool Network::SendMsg(char* str, int len)
{
	return false;
}