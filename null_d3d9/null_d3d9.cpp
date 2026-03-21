#ifndef INITGUID
#define INITGUID
#endif
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <d3d9.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>

// ---------------------------------------------------------------------------
// Debug logging
// ---------------------------------------------------------------------------
static void D3DLog(const char* fmt, ...) {
    FILE* f = fopen("C:\\d3d9_null.log", "a");
    if (f) {
        DWORD tick = GetTickCount();
        fprintf(f, "[%u.%03u] ", tick / 1000, tick % 1000);
        va_list args;
        va_start(args, fmt);
        vfprintf(f, fmt, args);
        va_end(args);
        fprintf(f, "\n");
        fclose(f);
    }
}

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
class NullSurface;
class NullTexture;
class NullVertexBuffer;
class NullIndexBuffer;
class NullVertexShader;
class NullPixelShader;
class NullVertexDeclaration;
class NullStateBlock;
class NullSwapChain;
class NullDevice;
class NullDirect3D9;

// ---------------------------------------------------------------------------
// Shared helper
// ---------------------------------------------------------------------------
static void FillDeviceCaps(D3DCAPS9* pCaps)
{
    memset(pCaps, 0, sizeof(D3DCAPS9));
    pCaps->DeviceType                  = D3DDEVTYPE_HAL;
    pCaps->AdapterOrdinal              = 0;
    pCaps->Caps                        = 0;
    pCaps->Caps2                       = 0x00080000L; // D3DCAPS2_CANRENDERWINDOWED
    pCaps->Caps3                       = 0;
    pCaps->PresentationIntervals       = D3DPRESENT_INTERVAL_IMMEDIATE | D3DPRESENT_INTERVAL_ONE;
    pCaps->CursorCaps                  = D3DCURSORCAPS_COLOR;
    pCaps->DevCaps                     = D3DDEVCAPS_HWTRANSFORMANDLIGHT | D3DDEVCAPS_DRAWPRIMITIVES2EX;
    pCaps->PrimitiveMiscCaps           = D3DPMISCCAPS_CULLNONE | D3DPMISCCAPS_CULLCW | D3DPMISCCAPS_CULLCCW;
    pCaps->RasterCaps                  = D3DPRASTERCAPS_ZTEST | D3DPRASTERCAPS_FOGVERTEX | D3DPRASTERCAPS_FOGTABLE;
    pCaps->TextureCaps                 = D3DPTEXTURECAPS_ALPHA | D3DPTEXTURECAPS_MIPMAP | D3DPTEXTURECAPS_PROJECTED;
    // TextureOpCaps + ShadeCaps needed for IsEnvironmentMappingSupported() in CCustomCarEnvMapPipeline
    pCaps->TextureOpCaps               = D3DTEXOPCAPS_BLENDFACTORALPHA | D3DTEXOPCAPS_MULTIPLYADD |
                                         D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_MODULATE;
    pCaps->ShadeCaps                   = D3DPSHADECAPS_SPECULARGOURAUDRGB | D3DPSHADECAPS_COLORGOURAUDRGB;
    pCaps->TextureFilterCaps           = D3DPTFILTERCAPS_MINFPOINT | D3DPTFILTERCAPS_MAGFPOINT |
                                         D3DPTFILTERCAPS_MINFLINEAR | D3DPTFILTERCAPS_MAGFLINEAR;
    pCaps->MaxTextureWidth             = 4096;
    pCaps->MaxTextureHeight            = 4096;
    pCaps->MaxVolumeExtent             = 256;
    pCaps->MaxTextureRepeat            = 8192;
    pCaps->MaxTextureAspectRatio       = 4096;
    pCaps->MaxAnisotropy               = 16;
    pCaps->MaxVertexIndex              = 0xFFFFFF;
    pCaps->MaxStreams                   = 16;
    pCaps->MaxStreamStride             = 256;
    pCaps->VertexShaderVersion         = D3DVS_VERSION(3, 0);
    pCaps->MaxVertexShaderConst        = 256;
    pCaps->PixelShaderVersion          = D3DPS_VERSION(3, 0);
    pCaps->PixelShader1xMaxValue       = 65504.0f;
    pCaps->MaxSimultaneousTextures     = 8;
    pCaps->MaxTextureBlendStages       = 8;
    pCaps->MaxActiveLights             = 8;
    pCaps->MaxUserClipPlanes           = 6;
    pCaps->MaxVertexBlendMatrices      = 4;
    pCaps->MaxVertexBlendMatrixIndex   = 0;
    pCaps->MaxPointSize                = 256.0f;
    pCaps->MaxPrimitiveCount           = 0xFFFFFF;
    pCaps->NumSimultaneousRTs          = 4;
    pCaps->VS20Caps.Caps               = 0;
    pCaps->VS20Caps.DynamicFlowControlDepth = 24;
    pCaps->VS20Caps.NumTemps           = 32;
    pCaps->VS20Caps.StaticFlowControlDepth  = 4;
    pCaps->PS20Caps.Caps               = 0;
    pCaps->PS20Caps.DynamicFlowControlDepth = 24;
    pCaps->PS20Caps.NumTemps           = 32;
    pCaps->PS20Caps.StaticFlowControlDepth  = 4;
    pCaps->PS20Caps.NumInstructionSlots = 512;
}

// ===========================================================================
// NullSurface : IDirect3DSurface9
// ===========================================================================
class NullSurface : public IDirect3DSurface9
{
    ULONG           m_refCount;
    void*           m_buffer;
    UINT            m_width;
    UINT            m_height;
    D3DFORMAT       m_format;
    IDirect3DDevice9* m_device; // stored, not AddRef'd

public:
    NullSurface(UINT w, UINT h, D3DFORMAT fmt, IDirect3DDevice9* dev)
        : m_refCount(1), m_buffer(nullptr), m_width(w), m_height(h), m_format(fmt), m_device(dev) {}

    ~NullSurface() { free(m_buffer); }

    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DSurface9) {
            *ppv = static_cast<IDirect3DSurface9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }

    // IDirect3DResource9
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override {
        if (!ppDevice) return E_POINTER;
        *ppDevice = m_device;
        if (m_device) m_device->AddRef();
        return S_OK;
    }
    STDMETHOD(SetPrivateData)(REFGUID, CONST void*, DWORD, DWORD) override { return S_OK; }
    STDMETHOD(GetPrivateData)(REFGUID, void*, DWORD*) override { return D3DERR_NOTFOUND; }
    STDMETHOD(FreePrivateData)(REFGUID) override { return S_OK; }
    STDMETHOD_(DWORD, SetPriority)(DWORD) override { return 0; }
    STDMETHOD_(DWORD, GetPriority)() override { return 0; }
    STDMETHOD_(void, PreLoad)() override {}
    STDMETHOD_(D3DRESOURCETYPE, GetType)() override { return D3DRTYPE_SURFACE; }

    // IDirect3DSurface9
    STDMETHOD(GetContainer)(REFIID, void**) override { return E_NOINTERFACE; }
    STDMETHOD(GetDesc)(D3DSURFACE_DESC* pDesc) override {
        if (!pDesc) return E_POINTER;
        memset(pDesc, 0, sizeof(*pDesc));
        pDesc->Format            = m_format;
        pDesc->Type              = D3DRTYPE_SURFACE;
        pDesc->Usage             = 0;
        pDesc->Pool              = D3DPOOL_DEFAULT;
        pDesc->MultiSampleType   = D3DMULTISAMPLE_NONE;
        pDesc->MultiSampleQuality= 0;
        pDesc->Width             = m_width;
        pDesc->Height            = m_height;
        return S_OK;
    }
    STDMETHOD(LockRect)(D3DLOCKED_RECT* pLockedRect, CONST RECT*, DWORD) override {
        if (!pLockedRect) return E_POINTER;
        if (!m_buffer) m_buffer = malloc(m_width * m_height * 4);
        pLockedRect->pBits = m_buffer;
        pLockedRect->Pitch = (INT)(m_width * 4);
        return S_OK;
    }
    STDMETHOD(UnlockRect)() override { return S_OK; }
    STDMETHOD(GetDC)(HDC* phdc) override { if (phdc) *phdc = nullptr; return S_OK; }
    STDMETHOD(ReleaseDC)(HDC) override { return S_OK; }
};

// ===========================================================================
// NullTexture : IDirect3DTexture9
// ===========================================================================
class NullTexture : public IDirect3DTexture9
{
    ULONG            m_refCount;
    IDirect3DDevice9* m_device;
    UINT             m_levels;
    NullSurface**    m_surfaces;
    UINT             m_width;
    UINT             m_height;
    D3DFORMAT        m_format;

public:
    NullTexture(UINT w, UINT h, UINT levels, D3DFORMAT fmt, IDirect3DDevice9* dev)
        : m_refCount(1), m_device(dev), m_width(w), m_height(h), m_format(fmt)
    {
        if (levels == 0) {
            UINT maxDim = (w > h) ? w : h;
            levels = 1;
            while (maxDim > 1) { maxDim >>= 1; levels++; }
        }
        m_levels = levels;
        m_surfaces = (NullSurface**)malloc(sizeof(NullSurface*) * m_levels);
        for (UINT i = 0; i < m_levels; i++) {
            UINT lw = w >> i; if (lw < 1) lw = 1;
            UINT lh = h >> i; if (lh < 1) lh = 1;
            m_surfaces[i] = new NullSurface(lw, lh, fmt, dev);
        }
    }
    ~NullTexture() {
        for (UINT i = 0; i < m_levels; i++) m_surfaces[i]->Release();
        free(m_surfaces);
    }

    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DTexture9 ||
            riid == IID_IDirect3DBaseTexture9 || riid == IID_IDirect3DResource9) {
            *ppv = static_cast<IDirect3DTexture9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }

    // IDirect3DResource9
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override {
        if (!ppDevice) return E_POINTER;
        *ppDevice = m_device;
        if (m_device) m_device->AddRef();
        return S_OK;
    }
    STDMETHOD(SetPrivateData)(REFGUID, CONST void*, DWORD, DWORD) override { return S_OK; }
    STDMETHOD(GetPrivateData)(REFGUID, void*, DWORD*) override { return D3DERR_NOTFOUND; }
    STDMETHOD(FreePrivateData)(REFGUID) override { return S_OK; }
    STDMETHOD_(DWORD, SetPriority)(DWORD) override { return 0; }
    STDMETHOD_(DWORD, GetPriority)() override { return 0; }
    STDMETHOD_(void, PreLoad)() override {}
    STDMETHOD_(D3DRESOURCETYPE, GetType)() override { return D3DRTYPE_TEXTURE; }

    // IDirect3DBaseTexture9
    STDMETHOD_(DWORD, SetLOD)(DWORD) override { return 0; }
    STDMETHOD_(DWORD, GetLOD)() override { return 0; }
    STDMETHOD_(DWORD, GetLevelCount)() override { return m_levels; }
    STDMETHOD(SetAutoGenFilterType)(D3DTEXTUREFILTERTYPE) override { return S_OK; }
    STDMETHOD_(D3DTEXTUREFILTERTYPE, GetAutoGenFilterType)() override { return D3DTEXF_NONE; }
    STDMETHOD_(void, GenerateMipSubLevels)() override {}

    // IDirect3DTexture9
    STDMETHOD(GetLevelDesc)(UINT Level, D3DSURFACE_DESC* pDesc) override {
        if (Level >= m_levels) return D3DERR_INVALIDCALL;
        return m_surfaces[Level]->GetDesc(pDesc);
    }
    STDMETHOD(GetSurfaceLevel)(UINT Level, IDirect3DSurface9** ppSurface) override {
        if (!ppSurface) return E_POINTER;
        if (Level >= m_levels) return D3DERR_INVALIDCALL;
        *ppSurface = m_surfaces[Level];
        m_surfaces[Level]->AddRef();
        return S_OK;
    }
    STDMETHOD(LockRect)(UINT Level, D3DLOCKED_RECT* pLockedRect, CONST RECT* pRect, DWORD Flags) override {
        if (Level >= m_levels) return D3DERR_INVALIDCALL;
        return m_surfaces[Level]->LockRect(pLockedRect, pRect, Flags);
    }
    STDMETHOD(UnlockRect)(UINT Level) override { return S_OK; }
    STDMETHOD(AddDirtyRect)(CONST RECT*) override { return S_OK; }
};

// ===========================================================================
// NullVertexBuffer : IDirect3DVertexBuffer9
// ===========================================================================
class NullVertexBuffer : public IDirect3DVertexBuffer9
{
    ULONG            m_refCount;
    IDirect3DDevice9* m_device;
    void*            m_buffer;
    UINT             m_length;
    DWORD            m_fvf;

public:
    NullVertexBuffer(UINT Length, DWORD FVF, IDirect3DDevice9* dev)
        : m_refCount(1), m_device(dev), m_length(Length), m_fvf(FVF)
    {
        m_buffer = malloc(Length);
        memset(m_buffer, 0, Length);
    }
    ~NullVertexBuffer() { free(m_buffer); }

    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DVertexBuffer9) {
            *ppv = static_cast<IDirect3DVertexBuffer9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }

    // IDirect3DResource9
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override {
        if (!ppDevice) return E_POINTER;
        *ppDevice = m_device;
        if (m_device) m_device->AddRef();
        return S_OK;
    }
    STDMETHOD(SetPrivateData)(REFGUID, CONST void*, DWORD, DWORD) override { return S_OK; }
    STDMETHOD(GetPrivateData)(REFGUID, void*, DWORD*) override { return D3DERR_NOTFOUND; }
    STDMETHOD(FreePrivateData)(REFGUID) override { return S_OK; }
    STDMETHOD_(DWORD, SetPriority)(DWORD) override { return 0; }
    STDMETHOD_(DWORD, GetPriority)() override { return 0; }
    STDMETHOD_(void, PreLoad)() override {}
    STDMETHOD_(D3DRESOURCETYPE, GetType)() override { return D3DRTYPE_VERTEXBUFFER; }

    // IDirect3DVertexBuffer9
    STDMETHOD(Lock)(UINT OffsetToLock, UINT, void** ppbData, DWORD) override {
        if (!ppbData) return E_POINTER;
        *ppbData = (BYTE*)m_buffer + OffsetToLock;
        return S_OK;
    }
    STDMETHOD(Unlock)() override { return S_OK; }
    STDMETHOD(GetDesc)(D3DVERTEXBUFFER_DESC* pDesc) override {
        if (!pDesc) return E_POINTER;
        memset(pDesc, 0, sizeof(*pDesc));
        pDesc->Format = D3DFMT_VERTEXDATA;
        pDesc->Type   = D3DRTYPE_VERTEXBUFFER;
        pDesc->Usage  = 0;
        pDesc->Pool   = D3DPOOL_DEFAULT;
        pDesc->Size   = m_length;
        pDesc->FVF    = m_fvf;
        return S_OK;
    }
};

// ===========================================================================
// NullIndexBuffer : IDirect3DIndexBuffer9
// ===========================================================================
class NullIndexBuffer : public IDirect3DIndexBuffer9
{
    ULONG            m_refCount;
    IDirect3DDevice9* m_device;
    void*            m_buffer;
    UINT             m_length;
    D3DFORMAT        m_format;

public:
    NullIndexBuffer(UINT Length, D3DFORMAT Format, IDirect3DDevice9* dev)
        : m_refCount(1), m_device(dev), m_length(Length), m_format(Format)
    {
        m_buffer = malloc(Length);
        memset(m_buffer, 0, Length);
    }
    ~NullIndexBuffer() { free(m_buffer); }

    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DIndexBuffer9) {
            *ppv = static_cast<IDirect3DIndexBuffer9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }

    // IDirect3DResource9
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override {
        if (!ppDevice) return E_POINTER;
        *ppDevice = m_device;
        if (m_device) m_device->AddRef();
        return S_OK;
    }
    STDMETHOD(SetPrivateData)(REFGUID, CONST void*, DWORD, DWORD) override { return S_OK; }
    STDMETHOD(GetPrivateData)(REFGUID, void*, DWORD*) override { return D3DERR_NOTFOUND; }
    STDMETHOD(FreePrivateData)(REFGUID) override { return S_OK; }
    STDMETHOD_(DWORD, SetPriority)(DWORD) override { return 0; }
    STDMETHOD_(DWORD, GetPriority)() override { return 0; }
    STDMETHOD_(void, PreLoad)() override {}
    STDMETHOD_(D3DRESOURCETYPE, GetType)() override { return D3DRTYPE_INDEXBUFFER; }

    // IDirect3DIndexBuffer9
    STDMETHOD(Lock)(UINT OffsetToLock, UINT, void** ppbData, DWORD) override {
        if (!ppbData) return E_POINTER;
        *ppbData = (BYTE*)m_buffer + OffsetToLock;
        return S_OK;
    }
    STDMETHOD(Unlock)() override { return S_OK; }
    STDMETHOD(GetDesc)(D3DINDEXBUFFER_DESC* pDesc) override {
        if (!pDesc) return E_POINTER;
        memset(pDesc, 0, sizeof(*pDesc));
        pDesc->Format = m_format;
        pDesc->Type   = D3DRTYPE_INDEXBUFFER;
        pDesc->Usage  = 0;
        pDesc->Pool   = D3DPOOL_DEFAULT;
        pDesc->Size   = m_length;
        return S_OK;
    }
};

// ===========================================================================
// NullVertexShader : IDirect3DVertexShader9
// ===========================================================================
class NullVertexShader : public IDirect3DVertexShader9
{
    ULONG            m_refCount;
    IDirect3DDevice9* m_device;

public:
    NullVertexShader(IDirect3DDevice9* dev) : m_refCount(1), m_device(dev) {}

    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DVertexShader9) {
            *ppv = static_cast<IDirect3DVertexShader9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override {
        if (!ppDevice) return E_POINTER;
        *ppDevice = m_device;
        if (m_device) m_device->AddRef();
        return S_OK;
    }
    STDMETHOD(GetFunction)(void*, UINT*) override { return E_NOTIMPL; }
};

// ===========================================================================
// NullPixelShader : IDirect3DPixelShader9
// ===========================================================================
class NullPixelShader : public IDirect3DPixelShader9
{
    ULONG            m_refCount;
    IDirect3DDevice9* m_device;

public:
    NullPixelShader(IDirect3DDevice9* dev) : m_refCount(1), m_device(dev) {}

    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DPixelShader9) {
            *ppv = static_cast<IDirect3DPixelShader9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override {
        if (!ppDevice) return E_POINTER;
        *ppDevice = m_device;
        if (m_device) m_device->AddRef();
        return S_OK;
    }
    STDMETHOD(GetFunction)(void*, UINT*) override { return E_NOTIMPL; }
};

// ===========================================================================
// NullVertexDeclaration : IDirect3DVertexDeclaration9
// ===========================================================================
class NullVertexDeclaration : public IDirect3DVertexDeclaration9
{
    ULONG            m_refCount;
    IDirect3DDevice9* m_device;

public:
    NullVertexDeclaration(IDirect3DDevice9* dev) : m_refCount(1), m_device(dev) {}

    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DVertexDeclaration9) {
            *ppv = static_cast<IDirect3DVertexDeclaration9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override {
        if (!ppDevice) return E_POINTER;
        *ppDevice = m_device;
        if (m_device) m_device->AddRef();
        return S_OK;
    }
    STDMETHOD(GetDeclaration)(D3DVERTEXELEMENT9*, UINT*) override { return E_NOTIMPL; }
};

// ===========================================================================
// NullStateBlock : IDirect3DStateBlock9
// ===========================================================================
class NullStateBlock : public IDirect3DStateBlock9
{
    ULONG            m_refCount;
    IDirect3DDevice9* m_device;

public:
    NullStateBlock(IDirect3DDevice9* dev) : m_refCount(1), m_device(dev) {}

    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DStateBlock9) {
            *ppv = static_cast<IDirect3DStateBlock9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override {
        if (!ppDevice) return E_POINTER;
        *ppDevice = m_device;
        if (m_device) m_device->AddRef();
        return S_OK;
    }
    STDMETHOD(Capture)() override { return S_OK; }
    STDMETHOD(Apply)() override { return S_OK; }
};

// ===========================================================================
// NullSwapChain : IDirect3DSwapChain9
// ===========================================================================
class NullSwapChain : public IDirect3DSwapChain9
{
    ULONG            m_refCount;
    IDirect3DDevice9* m_device;

public:
    NullSwapChain(IDirect3DDevice9* dev) : m_refCount(1), m_device(dev) {}

    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DSwapChain9) {
            *ppv = static_cast<IDirect3DSwapChain9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }
    STDMETHOD(Present)(CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*, DWORD) override { return S_OK; }
    STDMETHOD(GetFrontBufferData)(IDirect3DSurface9*) override { return S_OK; }
    STDMETHOD(GetBackBuffer)(UINT, D3DBACKBUFFER_TYPE, IDirect3DSurface9** ppBackBuffer) override {
        if (!ppBackBuffer) return E_POINTER;
        *ppBackBuffer = new NullSurface(800, 600, D3DFMT_X8R8G8B8, m_device);
        return S_OK;
    }
    STDMETHOD(GetRasterStatus)(D3DRASTER_STATUS* pRasterStatus) override {
        if (pRasterStatus) memset(pRasterStatus, 0, sizeof(*pRasterStatus));
        return S_OK;
    }
    STDMETHOD(GetDisplayMode)(D3DDISPLAYMODE* pMode) override {
        if (!pMode) return E_POINTER;
        pMode->Width  = 800;
        pMode->Height = 600;
        pMode->RefreshRate = 60;
        pMode->Format = D3DFMT_X8R8G8B8;
        return S_OK;
    }
    STDMETHOD(GetDevice)(IDirect3DDevice9** ppDevice) override {
        if (!ppDevice) return E_POINTER;
        *ppDevice = m_device;
        if (m_device) m_device->AddRef();
        return S_OK;
    }
    STDMETHOD(GetPresentParameters)(D3DPRESENT_PARAMETERS* pp) override {
        if (pp) memset(pp, 0, sizeof(*pp));
        return S_OK;
    }
};

// ===========================================================================
// NullDevice : IDirect3DDevice9
// ===========================================================================
class NullDevice : public IDirect3DDevice9
{
    ULONG            m_refCount;
    NullDirect3D9*   m_parent;
    NullSurface*     m_backBuffer;
    NullSurface*     m_depthStencil;

public:
    int m_callCount = 0;
    void LogMethod(const char* name) {
        m_callCount++;
        if (m_callCount <= 50 || m_callCount % 100 == 0)
            D3DLog("NullDevice::%s (call #%d)", name, m_callCount);
    }

    NullDevice(NullDirect3D9* parent)
        : m_refCount(1), m_parent(parent)
    {
        D3DLog("NullDevice constructor");
        m_backBuffer   = new NullSurface(800, 600, D3DFMT_X8R8G8B8, this);
        m_depthStencil = new NullSurface(800, 600, D3DFMT_D24S8, this);
    }
    ~NullDevice() {
        m_backBuffer->Release();
        m_depthStencil->Release();
    }

    // === IUnknown ===
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3DDevice9) {
            *ppv = static_cast<IDirect3DDevice9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }

    // === IDirect3DDevice9 methods in EXACT vtable order ===

    STDMETHOD(TestCooperativeLevel)() override { LogMethod("TestCooperativeLevel"); return D3D_OK; }

    STDMETHOD_(UINT, GetAvailableTextureMem)() override { return 256 * 1024 * 1024; }

    STDMETHOD(EvictManagedResources)() override { return S_OK; }

    STDMETHOD(GetDirect3D)(IDirect3D9** ppD3D9) override {
        if (!ppD3D9) return E_POINTER;
        *ppD3D9 = (IDirect3D9*)m_parent;
        ((IUnknown*)m_parent)->AddRef();
        return S_OK;
    }

    STDMETHOD(GetDeviceCaps)(D3DCAPS9* pCaps) override { LogMethod("GetDeviceCaps");
        if (!pCaps) return E_POINTER;
        FillDeviceCaps(pCaps);
        return S_OK;
    }

    STDMETHOD(GetDisplayMode)(UINT, D3DDISPLAYMODE* pMode) override {
        if (!pMode) return E_POINTER;
        pMode->Width  = 800;
        pMode->Height = 600;
        pMode->RefreshRate = 60;
        pMode->Format = D3DFMT_X8R8G8B8;
        return S_OK;
    }

    STDMETHOD(GetCreationParameters)(D3DDEVICE_CREATION_PARAMETERS* pParams) override {
        if (!pParams) return E_POINTER;
        memset(pParams, 0, sizeof(*pParams));
        pParams->DeviceType     = D3DDEVTYPE_HAL;
        pParams->AdapterOrdinal = 0;
        pParams->BehaviorFlags  = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        pParams->hFocusWindow   = NULL;
        return S_OK;
    }

    STDMETHOD(SetCursorProperties)(UINT, UINT, IDirect3DSurface9*) override { return S_OK; }

    STDMETHOD_(void, SetCursorPosition)(int, int, DWORD) override {}

    STDMETHOD_(BOOL, ShowCursor)(BOOL) override { return FALSE; }

    STDMETHOD(CreateAdditionalSwapChain)(D3DPRESENT_PARAMETERS*, IDirect3DSwapChain9** ppSwapChain) override {
        if (!ppSwapChain) return E_POINTER;
        *ppSwapChain = new NullSwapChain(this);
        return S_OK;
    }

    STDMETHOD(GetSwapChain)(UINT, IDirect3DSwapChain9** ppSwapChain) override {
        if (!ppSwapChain) return E_POINTER;
        *ppSwapChain = new NullSwapChain(this);
        return S_OK;
    }

    STDMETHOD_(UINT, GetNumberOfSwapChains)() override { return 1; }

    STDMETHOD(Reset)(D3DPRESENT_PARAMETERS*) override { LogMethod("Reset"); return D3D_OK; }

    STDMETHOD(Present)(CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*) override { return D3D_OK; }

    STDMETHOD(GetBackBuffer)(UINT, UINT, D3DBACKBUFFER_TYPE, IDirect3DSurface9** ppBackBuffer) override {
        if (!ppBackBuffer) return E_POINTER;
        m_backBuffer->AddRef();
        *ppBackBuffer = m_backBuffer;
        return S_OK;
    }

    STDMETHOD(GetRasterStatus)(UINT, D3DRASTER_STATUS* pStatus) override {
        if (pStatus) memset(pStatus, 0, sizeof(*pStatus));
        return S_OK;
    }

    STDMETHOD(SetDialogBoxMode)(BOOL) override { return S_OK; }

    STDMETHOD_(void, SetGammaRamp)(UINT, DWORD, CONST D3DGAMMARAMP*) override {}

    STDMETHOD_(void, GetGammaRamp)(UINT, D3DGAMMARAMP* pRamp) override {
        if (pRamp) memset(pRamp, 0, sizeof(*pRamp));
    }

    STDMETHOD(CreateTexture)(UINT Width, UINT Height, UINT Levels, DWORD, D3DFORMAT Format, D3DPOOL, IDirect3DTexture9** ppTexture, HANDLE*) override {
        if (!ppTexture) return E_POINTER;
        D3DLog("CreateTexture %ux%u levels=%u fmt=%d", Width, Height, Levels, (int)Format);
        *ppTexture = new NullTexture(Width, Height, Levels, Format, this);
        return S_OK;
    }

    STDMETHOD(CreateVolumeTexture)(UINT W, UINT H, UINT D, UINT Levels, DWORD, D3DFORMAT Fmt, D3DPOOL, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE*) override {
        D3DLog("CreateVolumeTexture %ux%ux%u levels=%u fmt=%d — STUBBED OK", W, H, D, Levels, (int)Fmt);
        if (!ppVolumeTexture) return E_POINTER;
        // Return nullptr but S_OK — caller should null-check
        *ppVolumeTexture = nullptr;
        return D3DERR_INVALIDCALL;
    }

    STDMETHOD(CreateCubeTexture)(UINT EdgeLength, UINT Levels, DWORD, D3DFORMAT Fmt, D3DPOOL, IDirect3DCubeTexture9** ppCubeTexture, HANDLE*) override {
        D3DLog("CreateCubeTexture edge=%u levels=%u fmt=%d — returning stub texture", EdgeLength, Levels, (int)Fmt);
        if (!ppCubeTexture) return E_POINTER;
        // Vehicles need cube textures for environment mapping. Return a regular
        // NullTexture cast to cube — RW only needs a non-null pointer, it won't
        // actually sample from it in headless mode.
        *ppCubeTexture = (IDirect3DCubeTexture9*)(new NullTexture(EdgeLength, EdgeLength, Levels, Fmt, this));
        return S_OK;
    }

    STDMETHOD(CreateVertexBuffer)(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE*) override {
        if (!ppVertexBuffer) return E_POINTER;
        D3DLog("CreateVertexBuffer len=%u usage=0x%x fvf=0x%x pool=%d", Length, Usage, FVF, (int)Pool);
        *ppVertexBuffer = new NullVertexBuffer(Length, FVF, this);
        return S_OK;
    }

    STDMETHOD(CreateIndexBuffer)(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE*) override {
        if (!ppIndexBuffer) return E_POINTER;
        D3DLog("CreateIndexBuffer len=%u usage=0x%x fmt=%d pool=%d", Length, Usage, (int)Format, (int)Pool);
        *ppIndexBuffer = new NullIndexBuffer(Length, Format, this);
        return S_OK;
    }

    STDMETHOD(CreateRenderTarget)(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE, DWORD, BOOL, IDirect3DSurface9** ppSurface, HANDLE*) override {
        if (!ppSurface) return E_POINTER;
        *ppSurface = new NullSurface(Width, Height, Format, this);
        return S_OK;
    }

    STDMETHOD(CreateDepthStencilSurface)(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE, DWORD, BOOL, IDirect3DSurface9** ppSurface, HANDLE*) override {
        if (!ppSurface) return E_POINTER;
        *ppSurface = new NullSurface(Width, Height, Format, this);
        return S_OK;
    }

    STDMETHOD(UpdateSurface)(IDirect3DSurface9*, CONST RECT*, IDirect3DSurface9*, CONST POINT*) override { return S_OK; }

    STDMETHOD(UpdateTexture)(IDirect3DBaseTexture9*, IDirect3DBaseTexture9*) override { return S_OK; }

    STDMETHOD(GetRenderTargetData)(IDirect3DSurface9*, IDirect3DSurface9*) override { return S_OK; }

    STDMETHOD(GetFrontBufferData)(UINT, IDirect3DSurface9*) override { return S_OK; }

    STDMETHOD(StretchRect)(IDirect3DSurface9*, CONST RECT*, IDirect3DSurface9*, CONST RECT*, D3DTEXTUREFILTERTYPE) override { return S_OK; }

    STDMETHOD(ColorFill)(IDirect3DSurface9*, CONST RECT*, D3DCOLOR) override { return S_OK; }

    STDMETHOD(CreateOffscreenPlainSurface)(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL, IDirect3DSurface9** ppSurface, HANDLE*) override {
        if (!ppSurface) return E_POINTER;
        *ppSurface = new NullSurface(Width, Height, Format, this);
        return S_OK;
    }

    STDMETHOD(SetRenderTarget)(DWORD, IDirect3DSurface9*) override { return S_OK; }

    STDMETHOD(GetRenderTarget)(DWORD, IDirect3DSurface9** ppRenderTarget) override {
        if (!ppRenderTarget) return E_POINTER;
        m_backBuffer->AddRef();
        *ppRenderTarget = m_backBuffer;
        return S_OK;
    }

    STDMETHOD(SetDepthStencilSurface)(IDirect3DSurface9*) override { return S_OK; }

    STDMETHOD(GetDepthStencilSurface)(IDirect3DSurface9** ppZStencilSurface) override {
        if (!ppZStencilSurface) return E_POINTER;
        m_depthStencil->AddRef();
        *ppZStencilSurface = m_depthStencil;
        return S_OK;
    }

    STDMETHOD(BeginScene)() override { LogMethod("BeginScene"); return D3D_OK; }

    STDMETHOD(EndScene)() override { return D3D_OK; }

    STDMETHOD(Clear)(DWORD, CONST D3DRECT*, DWORD, D3DCOLOR, float, DWORD) override { return D3D_OK; }

    STDMETHOD(SetTransform)(D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*) override { return S_OK; }

    STDMETHOD(GetTransform)(D3DTRANSFORMSTATETYPE, D3DMATRIX* pMatrix) override {
        if (pMatrix) memset(pMatrix, 0, sizeof(*pMatrix));
        return S_OK;
    }

    STDMETHOD(MultiplyTransform)(D3DTRANSFORMSTATETYPE, CONST D3DMATRIX*) override { return S_OK; }

    STDMETHOD(SetViewport)(CONST D3DVIEWPORT9*) override { LogMethod("SetViewport"); return S_OK; }

    STDMETHOD(GetViewport)(D3DVIEWPORT9* pViewport) override {
        if (pViewport) memset(pViewport, 0, sizeof(*pViewport));
        return S_OK;
    }

    STDMETHOD(SetMaterial)(CONST D3DMATERIAL9*) override { return S_OK; }

    STDMETHOD(GetMaterial)(D3DMATERIAL9* pMaterial) override {
        if (pMaterial) memset(pMaterial, 0, sizeof(*pMaterial));
        return S_OK;
    }

    STDMETHOD(SetLight)(DWORD, CONST D3DLIGHT9*) override { return S_OK; }

    STDMETHOD(GetLight)(DWORD, D3DLIGHT9* pLight) override {
        if (pLight) memset(pLight, 0, sizeof(*pLight));
        return S_OK;
    }

    STDMETHOD(LightEnable)(DWORD, BOOL) override { return S_OK; }

    STDMETHOD(GetLightEnable)(DWORD, BOOL* pEnable) override {
        if (pEnable) *pEnable = FALSE;
        return S_OK;
    }

    STDMETHOD(SetClipPlane)(DWORD, CONST float*) override { return S_OK; }

    STDMETHOD(GetClipPlane)(DWORD, float* pPlane) override {
        if (pPlane) memset(pPlane, 0, sizeof(float) * 4);
        return S_OK;
    }

    STDMETHOD(SetRenderState)(D3DRENDERSTATETYPE, DWORD) override { return S_OK; }

    STDMETHOD(GetRenderState)(D3DRENDERSTATETYPE, DWORD* pValue) override {
        if (pValue) *pValue = 0;
        return S_OK;
    }

    STDMETHOD(CreateStateBlock)(D3DSTATEBLOCKTYPE, IDirect3DStateBlock9** ppSB) override {
        if (!ppSB) return E_POINTER;
        *ppSB = new NullStateBlock(this);
        return S_OK;
    }

    STDMETHOD(BeginStateBlock)() override { return S_OK; }

    STDMETHOD(EndStateBlock)(IDirect3DStateBlock9** ppSB) override {
        if (!ppSB) return E_POINTER;
        *ppSB = new NullStateBlock(this);
        return S_OK;
    }

    STDMETHOD(SetClipStatus)(CONST D3DCLIPSTATUS9*) override { return S_OK; }

    STDMETHOD(GetClipStatus)(D3DCLIPSTATUS9* pClipStatus) override {
        if (pClipStatus) memset(pClipStatus, 0, sizeof(*pClipStatus));
        return S_OK;
    }

    STDMETHOD(GetTexture)(DWORD, IDirect3DBaseTexture9** ppTexture) override {
        if (ppTexture) *ppTexture = nullptr;
        return S_OK;
    }

    STDMETHOD(SetTexture)(DWORD, IDirect3DBaseTexture9*) override { return S_OK; }

    STDMETHOD(GetTextureStageState)(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD* pValue) override {
        if (pValue) *pValue = 0;
        return S_OK;
    }

    STDMETHOD(SetTextureStageState)(DWORD, D3DTEXTURESTAGESTATETYPE, DWORD) override { return S_OK; }

    STDMETHOD(GetSamplerState)(DWORD, D3DSAMPLERSTATETYPE, DWORD* pValue) override {
        if (pValue) *pValue = 0;
        return S_OK;
    }

    STDMETHOD(SetSamplerState)(DWORD, D3DSAMPLERSTATETYPE, DWORD) override { return S_OK; }

    STDMETHOD(ValidateDevice)(DWORD* pNumPasses) override {
        if (pNumPasses) *pNumPasses = 1;
        return S_OK;
    }

    STDMETHOD(SetPaletteEntries)(UINT, CONST PALETTEENTRY*) override { return S_OK; }

    STDMETHOD(GetPaletteEntries)(UINT, PALETTEENTRY* pEntries) override {
        if (pEntries) memset(pEntries, 0, sizeof(PALETTEENTRY) * 256);
        return S_OK;
    }

    STDMETHOD(SetCurrentTexturePalette)(UINT) override { return S_OK; }

    STDMETHOD(GetCurrentTexturePalette)(UINT* pPaletteNumber) override {
        if (pPaletteNumber) *pPaletteNumber = 0;
        return S_OK;
    }

    STDMETHOD(SetScissorRect)(CONST RECT*) override { return S_OK; }

    STDMETHOD(GetScissorRect)(RECT* pRect) override {
        if (pRect) memset(pRect, 0, sizeof(*pRect));
        return S_OK;
    }

    STDMETHOD(SetSoftwareVertexProcessing)(BOOL) override { return S_OK; }

    STDMETHOD_(BOOL, GetSoftwareVertexProcessing)() override { return FALSE; }

    STDMETHOD(SetNPatchMode)(float) override { return S_OK; }

    STDMETHOD_(float, GetNPatchMode)() override { return 0.0f; }

    STDMETHOD(DrawPrimitive)(D3DPRIMITIVETYPE, UINT, UINT) override { return S_OK; }

    STDMETHOD(DrawIndexedPrimitive)(D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT) override { return S_OK; }

    STDMETHOD(DrawPrimitiveUP)(D3DPRIMITIVETYPE, UINT, CONST void*, UINT) override { return S_OK; }

    STDMETHOD(DrawIndexedPrimitiveUP)(D3DPRIMITIVETYPE, UINT, UINT, UINT, CONST void*, D3DFORMAT, CONST void*, UINT) override { return S_OK; }

    STDMETHOD(ProcessVertices)(UINT, UINT, UINT, IDirect3DVertexBuffer9*, IDirect3DVertexDeclaration9*, DWORD) override { return S_OK; }

    STDMETHOD(CreateVertexDeclaration)(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) override {
        if (!ppDecl) return E_POINTER;
        D3DLog("CreateVertexDeclaration elements=%p", (const void*)pVertexElements);
        *ppDecl = new NullVertexDeclaration(this);
        return S_OK;
    }

    STDMETHOD(SetVertexDeclaration)(IDirect3DVertexDeclaration9*) override { return S_OK; }

    STDMETHOD(GetVertexDeclaration)(IDirect3DVertexDeclaration9** ppDecl) override {
        if (ppDecl) *ppDecl = nullptr;
        return S_OK;
    }

    STDMETHOD(SetFVF)(DWORD) override { return S_OK; }

    STDMETHOD(GetFVF)(DWORD* pFVF) override {
        if (pFVF) *pFVF = 0;
        return S_OK;
    }

    STDMETHOD(CreateVertexShader)(CONST DWORD*, IDirect3DVertexShader9** ppShader) override {
        if (!ppShader) return E_POINTER;
        *ppShader = new NullVertexShader(this);
        return S_OK;
    }

    STDMETHOD(SetVertexShader)(IDirect3DVertexShader9*) override { return S_OK; }

    STDMETHOD(GetVertexShader)(IDirect3DVertexShader9** ppShader) override {
        if (ppShader) *ppShader = nullptr;
        return S_OK;
    }

    STDMETHOD(SetVertexShaderConstantF)(UINT, CONST float*, UINT) override { return S_OK; }

    STDMETHOD(GetVertexShaderConstantF)(UINT, float* pConstantData, UINT Count) override {
        if (pConstantData) memset(pConstantData, 0, sizeof(float) * 4 * Count);
        return S_OK;
    }

    STDMETHOD(SetVertexShaderConstantI)(UINT, CONST int*, UINT) override { return S_OK; }

    STDMETHOD(GetVertexShaderConstantI)(UINT, int* pConstantData, UINT Count) override {
        if (pConstantData) memset(pConstantData, 0, sizeof(int) * 4 * Count);
        return S_OK;
    }

    STDMETHOD(SetVertexShaderConstantB)(UINT, CONST BOOL*, UINT) override { return S_OK; }

    STDMETHOD(GetVertexShaderConstantB)(UINT, BOOL* pConstantData, UINT Count) override {
        if (pConstantData) memset(pConstantData, 0, sizeof(BOOL) * Count);
        return S_OK;
    }

    STDMETHOD(SetStreamSource)(UINT, IDirect3DVertexBuffer9*, UINT, UINT) override { return S_OK; }

    STDMETHOD(GetStreamSource)(UINT, IDirect3DVertexBuffer9** ppStreamData, UINT* pOffsetInBytes, UINT* pStride) override {
        if (ppStreamData) *ppStreamData = nullptr;
        if (pOffsetInBytes) *pOffsetInBytes = 0;
        if (pStride) *pStride = 0;
        return S_OK;
    }

    STDMETHOD(SetStreamSourceFreq)(UINT, UINT) override { return S_OK; }

    STDMETHOD(GetStreamSourceFreq)(UINT, UINT* pSetting) override {
        if (pSetting) *pSetting = 1;
        return S_OK;
    }

    STDMETHOD(SetIndices)(IDirect3DIndexBuffer9*) override { return S_OK; }

    STDMETHOD(GetIndices)(IDirect3DIndexBuffer9** ppIndexData) override {
        if (ppIndexData) *ppIndexData = nullptr;
        return S_OK;
    }

    STDMETHOD(CreatePixelShader)(CONST DWORD*, IDirect3DPixelShader9** ppShader) override {
        if (!ppShader) return E_POINTER;
        *ppShader = new NullPixelShader(this);
        return S_OK;
    }

    STDMETHOD(SetPixelShader)(IDirect3DPixelShader9*) override { return S_OK; }

    STDMETHOD(GetPixelShader)(IDirect3DPixelShader9** ppShader) override {
        if (ppShader) *ppShader = nullptr;
        return S_OK;
    }

    STDMETHOD(SetPixelShaderConstantF)(UINT, CONST float*, UINT) override { return S_OK; }

    STDMETHOD(GetPixelShaderConstantF)(UINT, float* pConstantData, UINT Count) override {
        if (pConstantData) memset(pConstantData, 0, sizeof(float) * 4 * Count);
        return S_OK;
    }

    STDMETHOD(SetPixelShaderConstantI)(UINT, CONST int*, UINT) override { return S_OK; }

    STDMETHOD(GetPixelShaderConstantI)(UINT, int* pConstantData, UINT Count) override {
        if (pConstantData) memset(pConstantData, 0, sizeof(int) * 4 * Count);
        return S_OK;
    }

    STDMETHOD(SetPixelShaderConstantB)(UINT, CONST BOOL*, UINT) override { return S_OK; }

    STDMETHOD(GetPixelShaderConstantB)(UINT, BOOL* pConstantData, UINT Count) override {
        if (pConstantData) memset(pConstantData, 0, sizeof(BOOL) * Count);
        return S_OK;
    }

    STDMETHOD(DrawRectPatch)(UINT, CONST float*, CONST D3DRECTPATCH_INFO*) override { return S_OK; }

    STDMETHOD(DrawTriPatch)(UINT, CONST float*, CONST D3DTRIPATCH_INFO*) override { return S_OK; }

    STDMETHOD(DeletePatch)(UINT) override { return S_OK; }

    STDMETHOD(CreateQuery)(D3DQUERYTYPE, IDirect3DQuery9** ppQuery) override {
        if (ppQuery) *ppQuery = nullptr;
        return S_OK;
    }
};

// ===========================================================================
// NullDirect3D9 : IDirect3D9
// ===========================================================================
class NullDirect3D9 : public IDirect3D9
{
    ULONG m_refCount;

public:
    NullDirect3D9() : m_refCount(1) {}

    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void** ppv) override {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IDirect3D9) {
            *ppv = static_cast<IDirect3D9*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    STDMETHOD_(ULONG, AddRef)() override { return ++m_refCount; }
    STDMETHOD_(ULONG, Release)() override {
        ULONG r = --m_refCount;
        if (r == 0) delete this;
        return r;
    }

    // IDirect3D9
    STDMETHOD(RegisterSoftwareDevice)(void*) override { return S_OK; }

    STDMETHOD_(UINT, GetAdapterCount)() override { D3DLog("GetAdapterCount -> 1"); return 1; }

    STDMETHOD(GetAdapterIdentifier)(UINT, DWORD, D3DADAPTER_IDENTIFIER9* pIdentifier) override {
        if (!pIdentifier) return E_POINTER;
        memset(pIdentifier, 0, sizeof(*pIdentifier));
        strncpy(pIdentifier->Driver, "null_d3d9.dll", sizeof(pIdentifier->Driver) - 1);
        strncpy(pIdentifier->Description, "Null D3D9 Device", sizeof(pIdentifier->Description) - 1);
        strncpy(pIdentifier->DeviceName, "Null D3D9", sizeof(pIdentifier->DeviceName) - 1);
        pIdentifier->VendorId  = 0;
        pIdentifier->DeviceId  = 0;
        pIdentifier->SubSysId  = 0;
        pIdentifier->Revision  = 0;
        return S_OK;
    }

    STDMETHOD_(UINT, GetAdapterModeCount)(UINT, D3DFORMAT) override { return 5; }

    STDMETHOD(EnumAdapterModes)(UINT, D3DFORMAT, UINT Mode, D3DDISPLAYMODE* pMode) override {
        if (!pMode) return E_POINTER;
        // Report standard resolutions so the video mode dialog combo box is populated
        static const struct { UINT w, h; } modes[] = {
            {640, 480}, {800, 600}, {1024, 768}, {1280, 720}, {1920, 1080}
        };
        if (Mode >= 5) return D3DERR_INVALIDCALL;
        pMode->Width  = modes[Mode].w;
        pMode->Height = modes[Mode].h;
        pMode->RefreshRate = 60;
        pMode->Format = D3DFMT_X8R8G8B8;
        return S_OK;
    }

    STDMETHOD(GetAdapterDisplayMode)(UINT, D3DDISPLAYMODE* pMode) override {
        if (!pMode) return E_POINTER;
        pMode->Width  = 800;
        pMode->Height = 600;
        pMode->RefreshRate = 60;
        pMode->Format = D3DFMT_X8R8G8B8;
        return S_OK;
    }

    STDMETHOD(CheckDeviceType)(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, BOOL) override { return D3D_OK; }

    STDMETHOD(CheckDeviceFormat)(UINT, D3DDEVTYPE, D3DFORMAT, DWORD, D3DRESOURCETYPE, D3DFORMAT) override { return D3D_OK; }

    STDMETHOD(CheckDeviceMultiSampleType)(UINT, D3DDEVTYPE, D3DFORMAT, BOOL, D3DMULTISAMPLE_TYPE, DWORD*) override { return D3D_OK; }

    STDMETHOD(CheckDepthStencilMatch)(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, D3DFORMAT) override { return D3D_OK; }

    STDMETHOD(CheckDeviceFormatConversion)(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT) override { return D3D_OK; }

    STDMETHOD(GetDeviceCaps)(UINT, D3DDEVTYPE, D3DCAPS9* pCaps) override {
        if (!pCaps) return E_POINTER;
        FillDeviceCaps(pCaps);
        return S_OK;
    }

    STDMETHOD_(HMONITOR, GetAdapterMonitor)(UINT) override { return (HMONITOR)1; }

    STDMETHOD(CreateDevice)(UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPP, IDirect3DDevice9** ppReturnedDeviceInterface) override {
        D3DLog("CreateDevice called: Adapter=%u DeviceType=%u hWnd=%p Flags=0x%lX",
               Adapter, (unsigned)DeviceType, (void*)hFocusWindow, BehaviorFlags);
        if (pPP) {
            D3DLog("  PresentParams: BackBuffer=%ux%u Format=%d Windowed=%d SwapEffect=%d",
                   pPP->BackBufferWidth, pPP->BackBufferHeight, pPP->BackBufferFormat,
                   pPP->Windowed, pPP->SwapEffect);
        }
        if (!ppReturnedDeviceInterface) return E_POINTER;
        *ppReturnedDeviceInterface = new NullDevice(this);
        D3DLog("CreateDevice returning device=%p", (void*)*ppReturnedDeviceInterface);
        return S_OK;
    }
};

// ===========================================================================
// DllMain
// ===========================================================================
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        FILE* f = fopen("C:\\d3d9_loaded.log", "w");
        if (f) {
            fprintf(f, "null d3d9.dll DllMain DLL_PROCESS_ATTACH\n");
            fprintf(f, "  hInst=%p\n", (void*)hInst);
            char path[MAX_PATH];
            GetModuleFileNameA(hInst, path, sizeof(path));
            fprintf(f, "  path=%s\n", path);
            fprintf(f, "  GetTickCount=%lu\n", GetTickCount());
            fclose(f);
        }
        // Also start the d3d9_null.log
        D3DLog("null d3d9.dll loaded (DLL_PROCESS_ATTACH)");
    }
    return TRUE;
}

// ===========================================================================
// Exports
// ===========================================================================
extern "C" IDirect3D9* WINAPI Direct3DCreate9(UINT SDKVersion)
{
    D3DLog("Direct3DCreate9 called, SDKVersion=%u (expect %u=D3D_SDK_VERSION)", SDKVersion, D3D_SDK_VERSION);
    auto* obj = new NullDirect3D9();
    D3DLog("Direct3DCreate9 returning %p", (void*)obj);
    return obj;
}

extern "C" int WINAPI D3DPERF_BeginEvent(D3DCOLOR col, LPCWSTR name)
{
    (void)col; (void)name;
    return 0;
}

extern "C" int WINAPI D3DPERF_EndEvent()
{
    return 0;
}

extern "C" DWORD WINAPI D3DPERF_GetStatus()
{
    return 0;
}

extern "C" BOOL WINAPI D3DPERF_QueryRepeatFrame()
{
    return FALSE;
}

extern "C" void WINAPI D3DPERF_SetMarker(D3DCOLOR col, LPCWSTR name)
{
    (void)col; (void)name;
}

extern "C" void WINAPI D3DPERF_SetOptions(DWORD options)
{
    (void)options;
}

extern "C" void WINAPI D3DPERF_SetRegion(D3DCOLOR col, LPCWSTR name)
{
    (void)col; (void)name;
}

// (duplicate DllMain removed — the one above at line ~1179 handles DLL_PROCESS_ATTACH)
