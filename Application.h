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
   HWND                          hWnd;  // ���������� ����
   HINSTANCE                     hInst; // ���������� ����������
   WNDCLASSEX                    wc; // ��������� ������ ����
   LPCWSTR                       application_name; 
   LPCWSTR                       application_caption; // ��������� ����
   RECT                          window;
   DWORD                         dwStyle; // ����� ����
   int                           Width; // ������ ���� �� ���������
   int                           Height; // ������ ���� �� ���������
   void InitApplication();
   void Render();
public:
   
   HWND      GethWnd();  // ���������� ���������� ����
   HINSTANCE GethInst(); // ���������� ���������� ����������
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