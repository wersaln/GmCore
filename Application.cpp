#include <assert.h>
#include "Application.h"

#define SAFE_DELETE(p)       { if (p) { delete (p);     (p) = NULL; } }
#define SET_ACCESSOR( x, y )       inline void Set##y( x t )   { app_state.pCallback##y = t; };
#define GET_ACCESSOR( x, y )       inline x Get##y()           { return app_state.pCallback##y; };
#define GET_SET_ACCESSOR( x, y )   SET_ACCESSOR( x, y ) GET_ACCESSOR( x, y )

HRESULT WINAPI ApplicationCreateState();
void WINAPI ApplicationDestroyState();
LRESULT CALLBACK StaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
// описатель состояния приложения и обработчиков событий
class ApplicationState
{
protected:
	struct STATE
    {
        LPCALLBACKMSGPROC pCallbackWindowMsgFunc;
        LPCALLBACKKEYBOARD pCallbackKeyboardFunc;          // keyboard callback
        LPCALLBACKMOUSE pCallbackMouseFunc;                // mouse callback
		LPCALLBACKCREATED pCallbackCreatedFunc;            // created callback
		LPCALLBACKFRAMERENDER pCallbackFrameRenderFunc;   // render callback
		LPCALLBACKRESIZE pCallbackResizeFunc;
	};
	STATE app_state;
public:
    ApplicationState()  { Create(); }
    ~ApplicationState() { Destroy(); }
	void Create() { ZeroMemory( &app_state, sizeof( STATE ) );}
	void Destroy() { }

	GET_SET_ACCESSOR( LPCALLBACKMSGPROC, WindowMsgFunc );
    GET_SET_ACCESSOR( LPCALLBACKKEYBOARD, KeyboardFunc );
    GET_SET_ACCESSOR( LPCALLBACKMOUSE, MouseFunc );
	GET_SET_ACCESSOR( LPCALLBACKFRAMERENDER, FrameRenderFunc );
	GET_SET_ACCESSOR( LPCALLBACKRESIZE, ResizeFunc );
};

class MemoryHelper
{
public:
    MemoryHelper()  { ApplicationCreateState();  }
    ~MemoryHelper()  { ApplicationDestroyState(); }
};
ApplicationState *AppState;

HRESULT WINAPI ApplicationCreateState()
{
    if( AppState == NULL )
    {
        AppState = new ApplicationState;
        if( NULL == AppState )
            return E_OUTOFMEMORY;
    }
    return S_OK;
}
void WINAPI ApplicationDestroyState()
{
    SAFE_DELETE( AppState );
}
ApplicationState& GetApplicationState()
{
    // This class will auto create the memory when its first accessed and delete it after the program exits WinMain.
    // However the application can also call DXUTCreateState() & DXUTDestroyState() independantly if its wants 
    static MemoryHelper memory;
    assert(AppState != NULL );
    return *AppState;
}

void WINAPI ApplicationSetCallbackKeyboard( LPCALLBACKKEYBOARD pCallback )                                    { GetApplicationState().SetKeyboardFunc( pCallback ); }
void WINAPI ApplicationSetCallbackMouse( LPCALLBACKMOUSE pCallback, bool bIncludeMouseMove )                  { GetApplicationState().SetMouseFunc( pCallback ); }
void WINAPI ApplicationSetCallbackMsgProc( LPCALLBACKMSGPROC pCallback )                                      { GetApplicationState().SetWindowMsgFunc( pCallback );  }
void WINAPI ApplicationSetCallbackFrameRender( LPCALLBACKFRAMERENDER pCallback )                          { GetApplicationState().SetFrameRenderFunc( pCallback );  }
void WINAPI ApplicationSetCallbackResize( LPCALLBACKRESIZE pCallback )                                    { GetApplicationState().SetResizeFunc( pCallback );  }

RECT GetCenterRect(int Width, int Height)
{
	HDC DC = GetDC(0);
	int xSize = GetDeviceCaps(DC, HORZRES);
    int ySize = GetDeviceCaps(DC, VERTRES);
    ReleaseDC(0,DC);

	RECT new_rect =  {(xSize - Width) / 2,  (ySize - Height) / 2, Width, Height};
	return new_rect;
}
int GetScreenSizeX()
{
	HDC DC = GetDC(0);
	int xSize = GetDeviceCaps(DC, HORZRES);
    ReleaseDC(0,DC);
	return xSize;
}
int GetScreenSizeY()
{
	HDC DC = GetDC(0);
    int ySize = GetDeviceCaps(DC, VERTRES);
    ReleaseDC(0,DC);
    return ySize;
}
HWND Application::GethWnd()
{
	return hWnd;
}
HINSTANCE Application::GethInst()
{
	return hInst;
}
void Application::Render()
{
	//if( !GetDXUTState().GetRenderingPaused() ){
	double fTime, fAbsTime; float fElapsedTime;
    // Render the scene by calling the app's render callback
    LPCALLBACKFRAMERENDER pCallbackFrameRender = GetApplicationState().GetFrameRenderFunc();
	if( pCallbackFrameRender != NULL )
    {
        pCallbackFrameRender( fTime, fElapsedTime);
    }
}
void Application::Run()
{
	MSG msg;
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
		else
		    Render();
    }
}
void Application::InitApplication()
{
	Width   = GetScreenSizeX() / 2; 
    Height  = GetScreenSizeY() / 2;
    window  = GetCenterRect(Width, Height);
	dwStyle = WS_OVERLAPPEDWINDOW;//| WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	hInst   = ( HINSTANCE )GetModuleHandle( NULL );
	
	WNDCLASSEX new_wc =
    {
        sizeof( WNDCLASSEX ), CS_DBLCLKS, StaticWndProc, 0L, 0L,
        hInst, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW), NULL, NULL,
        application_name, LoadIcon(NULL, IDI_APPLICATION)
    };
	wc = new_wc;
}
Application::Application()
{
}
Application::Application(LPCWSTR APPLICATION_NAME, LPCWSTR APPLICATION_CAPTION)
{
	ApplicationCreateWindow(APPLICATION_NAME, APPLICATION_CAPTION);
}
void Application::ApplicationCreateWindow(LPCWSTR APPLICATION_NAME, LPCWSTR APPLICATION_CAPTION)
{
	application_name = APPLICATION_NAME;
	application_caption = APPLICATION_CAPTION;
	InitApplication();

    RegisterClassEx( &wc );

	hWnd = CreateWindow( application_name, application_caption, dwStyle, 
		                 window.left,  window.top, Width, Height,
                         NULL, NULL, wc.hInstance, NULL );
	
    ShowWindow( hWnd, SW_SHOWDEFAULT );
    UpdateWindow( hWnd );
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
}
Application::~Application()
{
	UnregisterClass( application_name, wc.hInstance );
}
LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

	if( uMsg == WM_KEYDOWN ||
        uMsg == WM_SYSKEYDOWN ||
        uMsg == WM_KEYUP ||
        uMsg == WM_SYSKEYUP )
    {
        bool bKeyDown = ( uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN );
        DWORD dwMask = ( 1 << 29 );
        bool bAltDown = ( ( lParam & dwMask ) != 0 );

        LPCALLBACKKEYBOARD pCallbackKeyboard = GetApplicationState().GetKeyboardFunc();
        if( pCallbackKeyboard )
            pCallbackKeyboard( ( UINT )wParam, bKeyDown, bAltDown);
    }
	 // Consolidate the mouse button messages and pass them to the app's mouse callback
    if( uMsg == WM_LBUTTONDOWN ||
        uMsg == WM_LBUTTONUP ||
        uMsg == WM_LBUTTONDBLCLK ||
        uMsg == WM_MBUTTONDOWN ||
        uMsg == WM_MBUTTONUP ||
        uMsg == WM_MBUTTONDBLCLK ||
        uMsg == WM_RBUTTONDOWN ||
        uMsg == WM_RBUTTONUP ||
        uMsg == WM_RBUTTONDBLCLK ||
        uMsg == WM_XBUTTONDOWN ||
        uMsg == WM_XBUTTONUP ||
        uMsg == WM_XBUTTONDBLCLK ||
        uMsg == WM_MOUSEWHEEL ||
        ( uMsg == WM_MOUSEMOVE ) )
    {
        int xPos = ( short )LOWORD( lParam );
        int yPos = ( short )HIWORD( lParam );

        if( uMsg == WM_MOUSEWHEEL )
        {
            // WM_MOUSEWHEEL passes screen mouse coords
            // so convert them to client coords
            POINT pt;
            pt.x = xPos; pt.y = yPos;
            //ScreenToClient( hWnd, &pt );
            xPos = pt.x; yPos = pt.y;
        }

        int nMouseWheelDelta = 0;
        if( uMsg == WM_MOUSEWHEEL )
            nMouseWheelDelta = ( short )HIWORD( wParam );

        int nMouseButtonState = LOWORD( wParam );
        bool bLeftButton = ( ( nMouseButtonState & MK_LBUTTON ) != 0 );
        bool bRightButton = ( ( nMouseButtonState & MK_RBUTTON ) != 0 );
        bool bMiddleButton = ( ( nMouseButtonState & MK_MBUTTON ) != 0 );
        bool bSideButton1 = ( ( nMouseButtonState & MK_XBUTTON1 ) != 0 );
        bool bSideButton2 = ( ( nMouseButtonState & MK_XBUTTON2 ) != 0 );

        LPCALLBACKMOUSE pCallbackMouse = GetApplicationState().GetMouseFunc();
        if( pCallbackMouse )
            pCallbackMouse( bLeftButton, bRightButton, bMiddleButton, bSideButton1, bSideButton2, nMouseWheelDelta,
                            xPos, yPos);
    }

	LPCALLBACKMSGPROC pCallbackMsgProc = GetApplicationState().GetWindowMsgFunc();
	if( pCallbackMsgProc )
    {
        LRESULT nResult = pCallbackMsgProc( hWnd, uMsg, wParam, lParam);
    }
	switch( uMsg )
    {
		case WM_MOUSEMOVE:
		{
			break;
		}
		case WM_KEYDOWN:
		{	
			switch (wParam) 
			{
				case VK_ESCAPE:
                {
                    //SendMessage( hWnd, WM_CLOSE, 0, 0 );
                    break;
                }
				default:
				break;
			}
			break;
		}
		case WM_PAINT:
		{
			double fTime, fAbsTime; float fElapsedTime;
			// Render the scene by calling the app's render callback
			LPCALLBACKFRAMERENDER pCallbackFrameRender = GetApplicationState().GetFrameRenderFunc();
			if( pCallbackFrameRender != NULL )
			{
				pCallbackFrameRender( fTime, fElapsedTime);
			}
			return 0;
		}
		case WM_DESTROY:
		{
            PostQuitMessage( 0 );
   			return 0;
		}
		case WM_CREATE:
			return 0;
		case WM_SIZE:
		{
			LPCALLBACKRESIZE pCallbackResize = GetApplicationState().GetResizeFunc();
			if( pCallbackMsgProc )
				pCallbackResize( LOWORD(lParam), HIWORD(lParam) );
			return 0;
		}
	    case WM_SYSKEYDOWN:
        {
            switch( wParam )
            {
                case VK_RETURN:
                {
                        // Toggle full screen upon alt-enter 
                        DWORD dwMask = ( 1 << 29 );
                        if( ( lParam & dwMask ) != 0 ) // Alt is down also
                        {
							// Toggle the full screen/window mode
                            return 0;
                        }
                }
            }
            break;
        }
    }

	if( ( uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP ) && wParam == VK_F10 )
        return 0;
    else
        return DefWindowProc( hWnd, uMsg, wParam, lParam );
}