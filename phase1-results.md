# Phase 1: Null D3D9 Device

A standalone stub `d3d9.dll` that implements all D3D9 COM interfaces as no-ops, allowing GTA:SA's RenderWare engine to initialize without a GPU or display.

## Quick Start

```bash
# Build (uses the same Docker image from Phase 0)
docker run --rm \
    -v ./null_d3d9:/src/null_d3d9:ro \
    -v ./scripts/build-null-d3d9.sh:/scripts/build-null-d3d9.sh:ro \
    -v ./build-output:/build \
    gta-reversed-build /scripts/build-null-d3d9.sh

# Output: build-output/d3d9.dll (~24KB PE32 x86 DLL)
```

Build takes ~5 seconds (single source file, no dependencies).

## What It Does

The game calls `LoadLibrary("d3d9.dll")` → `Direct3DCreate9()` → `IDirect3D9::CreateDevice()`. RenderWare then uses the device to create textures, surfaces, buffers, and issue draw calls. Our stub provides a fake device where every method succeeds but does nothing.

```
Game calls Direct3DCreate9()
  └── Returns NullDirect3D9
        ├── GetAdapterCount() → 1
        ├── GetAdapterDisplayMode() → 800x600 32-bit 60Hz
        ├── GetDeviceCaps() → plausible D3DCAPS9
        └── CreateDevice() → NullDevice
              ├── CreateTexture() → NullTexture (with lockable surfaces)
              ├── CreateVertexBuffer() → NullVertexBuffer (real malloc'd memory)
              ├── BeginScene/EndScene/Present → S_OK (no-op)
              ├── SetRenderState/SetTexture/... → S_OK (no-op)
              ├── DrawPrimitive/DrawIndexedPrimitive → S_OK (no-op)
              └── All ~120 other methods → S_OK / no-op
```

## How to Use

Place `d3d9.dll` in the game directory alongside `gta_sa.exe`. When running under Wine, override the system d3d9:

```bash
export WINEDLLOVERRIDES="d3d9=n"   # use native (our) d3d9.dll, not Wine's
wine gta_sa.exe
```

## File Structure

```
null_d3d9/
├── null_d3d9.cpp   # All 11 COM class implementations (1211 lines)
├── null_d3d9.def   # DLL export definitions
└── CMakeLists.txt  # Build configuration
```

## Implementation Details

### COM Classes (11 total)

| Class | Interface | Methods | Purpose |
|---|---|---|---|
| `NullDirect3D9` | `IDirect3D9` | 15 | Root object — adapter enumeration, device creation |
| `NullDevice` | `IDirect3DDevice9` | ~120 | Main device — all rendering calls |
| `NullTexture` | `IDirect3DTexture9` | 20 | Texture with lockable mip surfaces |
| `NullSurface` | `IDirect3DSurface9` | 15 | Lockable surface with real memory buffer |
| `NullVertexBuffer` | `IDirect3DVertexBuffer9` | 10 | Vertex data buffer |
| `NullIndexBuffer` | `IDirect3DIndexBuffer9` | 10 | Index data buffer |
| `NullVertexShader` | `IDirect3DVertexShader9` | 5 | Shader handle stub |
| `NullPixelShader` | `IDirect3DPixelShader9` | 5 | Shader handle stub |
| `NullVertexDeclaration` | `IDirect3DVertexDeclaration9` | 5 | Vertex format stub |
| `NullStateBlock` | `IDirect3DStateBlock9` | 5 | Render state snapshot stub |
| `NullSwapChain` | `IDirect3DSwapChain9` | 8 | Swap chain stub |

### Methods With Non-Trivial Implementations

Most methods return `S_OK` and do nothing. These are the exceptions:

**Memory-providing methods** — RenderWare writes data into locked buffers during asset loading:
- `NullSurface::LockRect` — lazy-allocates `width * height * 4` bytes, returns pointer via `D3DLOCKED_RECT`
- `NullVertexBuffer::Lock` — returns pointer into pre-allocated buffer (malloc'd at creation)
- `NullIndexBuffer::Lock` — same pattern

**Object-creating methods** — return new Null* objects via output pointers:
- `NullDevice::CreateTexture` — creates NullTexture with NullSurface per mip level
- `NullDevice::CreateVertexBuffer/CreateIndexBuffer` — allocate buffer memory
- `NullDevice::CreateRenderTarget/CreateDepthStencilSurface` — create NullSurface
- `NullDevice::CreateVertexShader/CreatePixelShader` — create stub objects
- `NullDirect3D9::CreateDevice` — creates the NullDevice

**Capability queries** — RenderWare checks these to decide code paths:
- `GetDeviceCaps` — returns plausible `D3DCAPS9` (VS 3.0, PS 3.0, 4096 max texture, 16x anisotropy)
- `GetAdapterDisplayMode` — returns 800x600 32-bit 60Hz
- `GetAdapterCount` — returns 1
- `TestCooperativeLevel` — returns `D3D_OK` (device is fine)
- `GetAvailableTextureMem` — returns 256MB

**Reference counting** — all objects track `AddRef`/`Release` correctly, `delete this` when refcount hits 0.

### DLL Exports

| Export | Ordinal | Purpose |
|---|---|---|
| `Direct3DCreate9` | @1 | Creates NullDirect3D9 object |
| `D3DPERF_BeginEvent` | @2 | Performance marker (no-op) |
| `D3DPERF_EndEvent` | @3 | Performance marker (no-op) |
| `D3DPERF_GetStatus` | @4 | Returns 0 |
| `D3DPERF_QueryRepeatFrame` | @5 | Returns FALSE |
| `D3DPERF_SetMarker` | @6 | No-op |
| `D3DPERF_SetOptions` | @7 | No-op |
| `D3DPERF_SetRegion` | @8 | No-op |

### Device Capabilities Reported

```
DeviceType:           D3DDEVTYPE_HAL
MaxTextureWidth:      4096
MaxTextureHeight:     4096
MaxAnisotropy:        16
VertexShaderVersion:  3.0
PixelShaderVersion:   3.0
MaxPrimitiveCount:    0xFFFFFF
MaxVertexIndex:       0xFFFF
MaxStreams:           16
```

## Why Not Use an Existing Library?

All existing D3D9 stub/wrapper projects are **proxies** that load the real system `d3d9.dll` underneath:
- [Software_D3D9](https://github.com/code-tom-code/Software_D3D9) — software renderer, but wraps real d3d9.dll
- [DirectX-Wrappers](https://github.com/elishacloud/DirectX-Wrappers) — complete proxy scaffold, forwards to real d3d9.dll
- [faked3d9](https://github.com/exolyte/faked3d9) — minimal stub, but `CreateDevice` deliberately fails

In a headless Docker/Wine environment with no GPU, the underlying real d3d9.dll (Wine's wined3d) fails at `CreateDevice` since there's no display adapter. A standalone implementation that never touches the real d3d9.dll is required.

## Build Details

- Compiler: MSVC 19.50 via Wine (same Docker image as Phase 0)
- Source: single `null_d3d9.cpp` (~1280 lines)
- No dependencies beyond `d3d9.h` from the Windows SDK
- Output: ~24KB DLL
- License: our own code, no third-party code used
