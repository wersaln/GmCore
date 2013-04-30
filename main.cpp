#include "Application.h"
#include "Graphics.h"
#include "Network.h"
Application app(L"D3D project", L"Main window");
Graphics d3dGraphics;
Network net(5300);
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	net.MsgHandler(hWnd, msg, wParam, lParam);
    return 0;
}
void  CALLBACK KeyPress( UINT nChar, bool bKeyDown, bool bAltDown)
{
	switch (nChar) 
	{
		case VK_ESCAPE:
        {
			SendMessage( app.GethWnd(), WM_CLOSE, 0, 0 );
            break;
        }
	}
}
void  CALLBACK Render( double fTime, float fElapsedTime)
{
	d3dGraphics.Render();
	//SendMessage( app.GethWnd(), WM_CLOSE, 0, 0 );
}
INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{
	d3dGraphics.InitD3D(app.GethWnd());
	//net.ClientInit(app.GethWnd(), "localhost");
	ApplicationSetCallbackKeyboard(KeyPress);
	ApplicationSetCallbackFrameRender(Render);
	ApplicationSetCallbackMsgProc(MsgProc);
	app.Run();
	return 0;
}