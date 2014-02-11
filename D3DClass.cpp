#include "D3DClass.h"

D3DClass::D3DClass(){
    m_SwapChain             = nullptr;
    m_Device                = nullptr;
    m_DeviceContext         = nullptr;
    m_RenderTargetView      = nullptr;
    m_DepthStencilBuffer    = nullptr;
    m_DepthStencilState     = nullptr;
    m_DepthStencilView      = nullptr;
    m_RasterState           = nullptr;
}

D3DClass::~D3DClass() {};

bool D3DClass::Initialize( int ScreenWidth, int ScreenHeight, bool VSync_Enabled, HWND hWnd, bool FullScreen, float ScreenDepth, float ScreenNear ) {
    HRESULT result;
    IDXGIFactory* Factory;
    IDXGIAdapter* Adapter;
    IDXGIOutput* AdapterOutput;
    unsigned int numModes, i, numerator, denominator, StringLength;
    DXGI_MODE_DESC* DisplayModeList;
    DXGI_ADAPTER_DESC AdapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC SwapChainDescription;
    D3D_FEATURE_LEVEL FeatureLevel;
    ID3D11Texture2D* BackBufferPtr;
    D3D11_TEXTURE2D_DESC DepthBufferDescription;
    D3D11_DEPTH_STENCIL_DESC DepthStencilDescription;
    D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDescription;
    D3D11_RASTERIZER_DESC RasterDescription;
    D3D11_VIEWPORT ViewPort;
    float FieldOfView, ScreenAspect;

    m_VSync_Enabled = VSync_Enabled;
    result = CreateDXGIFactory( __uuidof( IDXGIFactory ), (void**) &Factory );
    if ( FAILED(result) ) return false;
    result = Factory->EnumAdapters( 0, &Adapter );
    if ( FAILED(result) ) return false;
    result = Adapter->EnumOutputs( 0, &AdapterOutput );
    if ( FAILED(result) ) return false;
    result = AdapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr );
    if ( FAILED(result) ) return false;
    DisplayModeList = new DXGI_MODE_DESC[numModes];
    if ( !DisplayModeList ) return false;
    result = AdapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, DisplayModeList );
    if ( FAILED(result) ) return false;

    for ( i = 0; i < numModes; ++i ) {
        if ( DisplayModeList[i].Height == (unsigned int)ScreenHeight && DisplayModeList[i].Width == (unsigned int)ScreenWidth  ) {
            numerator = DisplayModeList[i].RefreshRate.Numerator;
            denominator = DisplayModeList[i].RefreshRate.Denominator;
        }
    }

    result = Adapter->GetDesc( &AdapterDesc );
    if ( FAILED(result) ) return false;

    error = wcstombs_s( &StringLength, m_VideoCardDescription, 128, AdapterDesc.Description, 128 );

    delete [] DisplayModeList;
    DisplayModeList = nullptr;
    AdapterOutput->Release();
    AdapterOutput = nullptr;
    Adapter->Release();
    Adapter = nullptr;
    Factory->Release();
    Factory = nullptr;

    ZeroMemory( &SwapChainDescription, sizeof(SwapChainDescription) );
    SwapChainDescription.BufferCount = 1;
    SwapChainDescription.BufferDesc.Width = ScreenWidth;
    SwapChainDescription.BufferDesc.Height = ScreenHeight;
    SwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    if ( m_VSync_Enabled ) {
        SwapChainDescription.BufferDesc.RefreshRate.Numerator = numerator;
        SwapChainDescription.BufferDesc.RefreshRate.Denominator = denominator;
    } else {
        SwapChainDescription.BufferDesc.RefreshRate.Numerator = 0;
        SwapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
    }
    SwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDescription.OutputWindow = hWnd;
    //turning multisampling off
    SwapChainDescription.SampleDesc.Count = 1;
    SwapChainDescription.SampleDesc.Quality = 0;
    SwapChainDescription.Windowed = !FullScreen; 
    SwapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    SwapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    SwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    SwapChainDescription.Flags = 0;

    FeatureLevel = D3D_FEATURE_LEVEL_11_0;

    result = D3D11CreateDeviceAndSwapChain( nullptr, D3D_DRIVER_TYPE_REFERENCE, nullptr, 0, &FeatureLevel, 1, D3D11_SDK_VERSION, &SwapChainDescription, &m_SwapChain, &m_Device, nullptr, &m_DeviceContext );
    if ( FAILED(result) ) return false;

    result = m_SwapChain->GetBuffer( 0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferPtr );
    if ( FAILED(result) ) return false;
    result = m_Device->CreateRenderTargetView( BackBufferPtr, nullptr, &m_RenderTargetView );
    if ( FAILED(result) ) return false;
    BackBufferPtr->Release();
    BackBufferPtr = nullptr;

    ZeroMemory( &DepthBufferDescription, sizeof(DepthBufferDescription) );
    DepthBufferDescription.Width = ScreenWidth;
    DepthBufferDescription.Height = ScreenHeight;
    DepthBufferDescription.MipLevels = 1;
    DepthBufferDescription.ArraySize = 1;
    DepthBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DepthBufferDescription.SampleDesc.Count = 1;
    DepthBufferDescription.SampleDesc.Quality = 0;
    DepthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
    DepthBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    DepthBufferDescription.CPUAccessFlags = 0;
    DepthBufferDescription.MiscFlags = 0;

    result = m_Device->CreateTexture2D( &DepthBufferDescription, nullptr, &m_DepthStencilBuffer );
    if ( FAILED(result) ) return false;

    ZeroMemory( &DepthStencilDescription, sizeof(DepthStencilDescription) );

    DepthStencilDescription.DepthEnable = true;
    DepthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;

    DepthStencilDescription.StencilEnable = true;
    DepthStencilDescription.StencilReadMask = 0xFF;
    DepthStencilDescription.StencilWriteMask = 0xFF;

    DepthStencilDescription.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilDescription.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    DepthStencilDescription.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilDescription.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    DepthStencilDescription.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilDescription.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    DepthStencilDescription.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    DepthStencilDescription.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = m_Device->CreateDepthStencilState( &DepthStencilDescription, &m_DepthStencilState );
    m_DeviceContext->OMSetDepthStencilState( m_DepthStencilState, 1 );

    ZeroMemory( &DepthStencilViewDescription, sizeof(DepthStencilViewDescription) );
    DepthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DepthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    DepthStencilViewDescription.Texture2D.MipSlice = 0;

    result = m_Device->CreateDepthStencilView( m_DepthStencilBuffer, &DepthStencilViewDescription, &m_DepthStencilView );
    if ( FAILED(result) ) return false;

    m_DeviceContext->OMSetRenderTargets( 1, &m_RenderTargetView, m_DepthStencilView );

    RasterDescription.AntialiasedLineEnable = false;
    RasterDescription.CullMode = D3D11_CULL_BACK;
    RasterDescription.DepthBias = 0;
    RasterDescription.DepthBiasClamp = 0.0f;
    RasterDescription.DepthClipEnable = true;
    RasterDescription.FillMode = D3D11_FILL_SOLID;
    RasterDescription.FrontCounterClockwise = false;
    RasterDescription.MultisampleEnable = false;
    RasterDescription.ScissorEnable = false;
    RasterDescription.SlopeScaledDepthBias = 0.0f;

    result = m_Device->CreateRasterizerState( &RasterDescription, &m_RasterState );
    if ( FAILED(result) ) return false;
    m_DeviceContext->RSSetState( m_RasterState );

    ViewPort.Width = (float)ScreenWidth;
    ViewPort.Height = (float)ScreenHeight;
    ViewPort.MinDepth = 0.0f;
    ViewPort.MaxDepth = 1.0f;
    ViewPort.TopLeftX = 0.0f;
    ViewPort.TopLeftY = 0.0f;

    m_DeviceContext->RSSetViewports( 1, & ViewPort );

    FieldOfView = (float)D3DX_PI /4.0f;
    ScreenAspect = (float)ScreenWidth / (float)ScreenHeight;

    D3DXMatrixPerspectiveFovLH( &m_ProjectionMatrix, FieldOfView, ScreenAspect, ScreenNear, ScreenDepth );
    D3DXMatrixIdentity( &m_WorldMatrix);
    D3DXMatrixOrthoLH( &m_OrthoMatrix, (float)ScreenWidth, (float)ScreenHeight, ScreenNear, ScreenDepth );
    return true;
}

void D3DClass::Shutdown() {
    if (m_SwapChain) m_SwapChain->SetFullscreenState( false, nullptr );
    if (m_RasterState) { m_RasterState->Release(); m_RasterState = nullptr; }
    if (m_DepthStencilView) { m_DepthStencilView->Release(); m_DepthStencilView = nullptr; }
    if (m_DepthStencilState) { m_DepthStencilState->Release(); m_DepthStencilState = nullptr; }
    if (m_DepthStencilBuffer) { m_DepthStencilBuffer->Release(); m_DepthStencilBuffer = nullptr; }
    if (m_RenderTargetView) { m_RenderTargetView->Release(); m_RenderTargetView = nullptr; }
    if (m_DeviceContext) { m_DeviceContext->Release(); m_DeviceContext = nullptr; }
    if (m_Device) { m_Device->Release(); m_Device = nullptr; }
    if (m_SwapChain) { m_SwapChain->Release(); m_SwapChain = nullptr; }
    return;
}

void D3DClass::BeginScene( float Red, float Green, float Blue, float Alpha ){
    float Color[4];
    Color[0] = Red;
    Color[1] = Green;
    Color[2] = Blue;
    Color[3] = Alpha;

    m_DeviceContext->ClearRenderTargetView( m_RenderTargetView, Color );
    m_DeviceContext->ClearDepthStencilView( m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
}

void D3DClass::EndScene() {
    if ( m_VSync_Enabled ) {
        m_SwapChain->Present(1,0);
    } else {
        m_SwapChain->Present(0,0);
    }
}

ID3D11Device* D3DClass::GetDevice() { return m_Device; }
ID3D11DeviceContext* D3DClass::GetDeviceContext() { return m_DeviceContext; }
void D3DClass::GetProjectionMatrix( D3DXMATRIX& ProjectionMatrix ) { ProjectionMatrix = m_ProjectionMatrix; }
void D3DClass::GetWorldMatrix( D3DXMATRIX& WorldMatrix ) { WorldMatrix = m_WorldMatrix; }
void D3DClass::GetOrthoMatrix( D3DXMATRIX& OrthoMatrix ) { OrthoMatrix = m_OrthoMatrix; }

void D3DClass::GetVideoCardInfo( char* CardName, int& Memory ) {
    strcpy_s( CardName, 128, m_VideoCardDescription );
    Memory = m_VideoCardMemory;
}
