#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H

// Direct3D9 libs
#pragma comment( lib, "dxerr.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3d10.lib" )
#if defined(DEBUG) || defined(_DEBUG)
#pragma comment( lib, "d3dx9d.lib" )
#pragma comment( lib, "d3dx10d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#pragma comment( lib, "d3dx10.lib" )
#endif
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "comctl32.lib" )

// Direct3D9 includes
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>
#include <windows.h>


class Graphics
{
protected:
	HWND                              hWnd;          // Дескриптор родительского окна

    LPDIRECT3D9                       g_pD3D9;        // Объект Direct3D
    LPDIRECT3DDEVICE9                 g_pD3D9Device;  // Объект устройства

    ID3DXSprite*                      g_pSprite;     // Объект двухмерного спрайта

	LPD3DXMESH          g_pMesh; // Our mesh object in sysmem
    D3DMATERIAL9*       g_pMeshMaterials; // Materials for our mesh
    LPDIRECT3DTEXTURE9* g_pMeshTextures; // Textures for our mesh
    DWORD               g_dwNumMaterials;   // Number of mesh materials

    D3DDISPLAYMODE                    d3ddm;         // Свойства видеорежима

    BOOL                              fWindowed;     // Флаг включения оконного режима
    BOOL                              g_ZBuffer;     // Флаг использования Z-буфера
    BOOL                              g_HAL;         // Флаг аппаратного ускорения
	
    long                              g_Width;       // Ширина для видеорежима
    long                              g_Height;      // Высота для видеорежима
    char                              BPP;           // Количество бит в пикселе
	VOID SetupMatrices();
public:
	BOOL GetHAL();
    BOOL GetZBuffer();

    Graphics(); // Конструктор
    ~Graphics(); // Деструктор
	
	bool Graphics::Load3DModel(WCHAR* MeshFile, WCHAR* TextureFile);
	HRESULT InitD3D(HWND hWnd);
	void Resize(int x, int y);
	void Render();
};

#endif