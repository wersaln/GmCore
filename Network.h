#include <windows.h>
#include <winsock.h>
struct Connection
{
	int ID;
	SOCKET socket;
};
class Network
{
private:
	int port;
	HWND hWnd;
    Connection connection;
	char *ipaddress;
public:
	Network(int port);
	~Network();
	void ClientInit( HWND hwnd, char *ip );
	void MsgHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	bool SendMsg(char* str, int len);
};