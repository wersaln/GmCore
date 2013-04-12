#pragma once
#ifndef APPLICATION_H
#define APPLICATION_H

#include <windows.h>

#pragma once

typedef LRESULT (CALLBACK *LPCALLBACKMSGPROC)( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
typedef void    (CALLBACK *LPCALLBACKKEYBOARD)( UINT nChar, bool bKeyDown, bool bAltDown);
typedef void    (CALLBACK *LPCALLBACKMOUSE)( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown, bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta, int xPos, int yPos);
typedef void    (CALLBACK *LPCALLBACKFRAMERENDER)( double fTime, float fElapsedTime);
typedef void    (CALLBACK *LPCALLBACKRESIZE)( int width, int height );
typedef void    (CALLBACK *LPCALLBACKCREATED)( );


class Application
{
private: 
   HWND                          hWnd;  // Дескриптор окна
   HINSTANCE                     hInst; // Дескриптор экземпляра
   WNDCLASSEX                    wc; // Структура класса окна
   LPCWSTR                       application_name; 
   LPCWSTR                       application_caption; // Заголовок окна
   RECT                          window;
   DWORD                         dwStyle; // Стиль окна
   int                           Width; // Ширина окна по умолчанию
   int                           Height; // Высота окна по умолчанию
   void InitApplication();
   void Render();
public:
   
   HWND      GethWnd();  // Возвращает дескриптор окна
   HINSTANCE GethInst(); // Возвращает дескриптор экземпляра
   void Run();
   void ApplicationCreateWindow(LPCWSTR APPLICATION_NAME, LPCWSTR APPLICATION_CAPTION);
   Application(LPCWSTR APPLICATION_NAME, LPCWSTR APPLICATION_CAPTION);
   Application();
   ~Application();
};

// General callbacks
void WINAPI ApplicationSetCallbackKeyboard( LPCALLBACKKEYBOARD pCallback);
void WINAPI ApplicationSetCallbackMouse( LPCALLBACKMOUSE pCallback, bool bIncludeMouseMove = false);
void WINAPI ApplicationSetCallbackMsgProc( LPCALLBACKMSGPROC pCallback);
void WINAPI ApplicationSetCallbackFrameRender( LPCALLBACKFRAMERENDER pCallback );                          
void WINAPI ApplicationSetCallbackResize( LPCALLBACKRESIZE pCallback );       

#endif