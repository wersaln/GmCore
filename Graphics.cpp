#include <assert.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "Graphics.h"
//#define SAFE_DELETE(p)       { if (p) { delete (p);     (p) = NULL; } }
//
//HRESULT WINAPI GraphicsCreateState();
//void WINAPI GraphicsDestroyState();
//
//class GraphicsState
//{
//protected:
//	struct STATE
//    {
//	};
//	STATE app_state;
//public:
//    GraphicsState()  { Create(); }
//    ~GraphicsState() { Destroy(); }
//	void Create() { ZeroMemory( &app_state, sizeof( STATE ) );}
//	void Destroy() { }
//};
//
//class MemoryHelper
//{
//public:
//    MemoryHelper()  { GraphicsCreateState();  }
//    ~MemoryHelper()  { GraphicsDestroyState(); }
//};
//GraphicsState *GrState;
//
//HRESULT WINAPI GraphicsCreateState()
//{
//    if( GrState == NULL )
//    {
//        GrState = new  GraphicsState;
//        if( NULL == GrState )
//            return E_OUTOFMEMORY;
//    }
//    return S_OK;
//}
//void WINAPI GraphicsDestroyState()
//{
//    SAFE_DELETE( GrState );
//}
//GraphicsState& GetGraphicsState()
//{
//    // This class will auto create the memory when its first accessed and delete it after the program exits WinMain.
//    // However the application can also call DXUTCreateState() & DXUTDestroyState() independantly if its wants 
//    static MemoryHelper memory;
//    assert(GrState != NULL );
//    return *GrState;
//}

//---------------------------------------------------------------------------------------
//Graphics class methods
//---------------------------------------------------------------------------------------
Graphics::Graphics()
{
    fWindowed = true;
}
Graphics::~Graphics()
{
	if( g_pMeshMaterials != NULL )
        delete[] g_pMeshMaterials;

    if( g_pMeshTextures )
    {
        for( DWORD i = 0; i < g_dwNumMaterials; i++ )
        {
            if( g_pMeshTextures[i] )
                g_pMeshTextures[i]->Release();
        }
        delete[] g_pMeshTextures;
    }
    if( g_pMesh != NULL )
        g_pMesh->Release();

    if( g_pD3D9Device != NULL )
        g_pD3D9Device->Release();

    if( g_pD3D9 != NULL )
        g_pD3D9->Release();
}
HRESULT Graphics::InitD3D(HWND hWnd)
{
	// Create the D3D object.
    if( NULL == ( g_pD3D9 = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Since we are now
    // using more complex geometry, we will create a device with a zbuffer.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = fWindowed;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    // Create the D3DDevice
    if( FAILED( g_pD3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pD3D9Device ) ) )
    {
        return E_FAIL;
    }

    // Turn on the zbuffer
    g_pD3D9Device->SetRenderState( D3DRS_ZENABLE, TRUE );

    // Turn on ambient lighting 
    g_pD3D9Device->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	return S_OK;
}
void  Graphics::Resize(int x, int y)
{
}
void Graphics::SetupMatrices()
{
    // Set up world matrix
    D3DXMATRIXA16 matWorld;
    D3DXMatrixRotationY( &matWorld, timeGetTime() / 1000.0f );
    g_pD3D9Device->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the 
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pD3D9Device->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f );
    g_pD3D9Device->SetTransform( D3DTS_PROJECTION, &matProj );
}
void  Graphics::Render()
{
	    // Clear the backbuffer and the zbuffer
    g_pD3D9Device->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB( 0, 255, 255 ), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( g_pD3D9Device->BeginScene() ) )
    {
        // Setup the world, view, and projection matrices
        SetupMatrices();

        // Meshes are divided into subsets, one for each material. Render them in
        // a loop
        for( DWORD i = 0; i < g_dwNumMaterials; i++ )
        {
            // Set the material and texture for this subset
            g_pD3D9Device->SetMaterial( &g_pMeshMaterials[i] );
            g_pD3D9Device->SetTexture( 0, g_pMeshTextures[i] );

            // Draw the mesh subset
            g_pMesh->DrawSubset( i );
        }

        // End the scene
        g_pD3D9Device->EndScene();
    }

    // Present the backbuffer contents to the display
    g_pD3D9Device->Present( NULL, NULL, NULL, NULL );
}
