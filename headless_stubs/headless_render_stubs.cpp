// headless_render_stubs.cpp — Patches RenderWare render functions to no-ops.
// Compiled into gta_reversed.asi via build-time patching (not part of upstream repo).
//
// Called from DllMain after hooks are installed. Overwrites the render function
// entry points with JMPs to stub functions, so the game loop runs without
// doing any CPU-side rendering work.
//
// Also replaces RsRwInitialize and psSelectDevice with logged, headless-friendly
// versions that call RW functions at their known game addresses.

#include "StdInc.h"

// ---------------------------------------------------------------------------
// Init logging — writes to C:\rw_init.log for tracing the startup sequence
// ---------------------------------------------------------------------------

static void InitLog(const char* fmt, ...) {
    FILE* f = fopen("C:\\rw_init.log", "a");
    if (f) {
        // Timestamp
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
// Stub functions — all __cdecl to match the game's calling convention
// ---------------------------------------------------------------------------

// Forward declaration — soak_test.cpp (if present)
extern void SoakTestOnFrame();

static void* __cdecl StubReturnFirstArg(void* arg, ...) { return arg; }
static void  __cdecl StubVoidNoOp(...) {}
static int   __cdecl StubReturnTrue(...) { return 1; }

// Win32-only file append — no CRT dependency (safe for threads with TLS zeroed)
static void Win32AppendLog(const char* path, const char* text) {
    HANDLE h = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(h, text, (DWORD)strlen(text), &written, NULL);
        CloseHandle(h);
    }
}

// Force-start new game if stuck at frontend idle (state 7) with no save
// This is called from the render stubs on each frame to check game state
static void CheckAndForceNewGame() {
    static bool s_forced = false;
    if (s_forced) return;

    int gs = *(int*)0xC8D4C0; // gGameState
    if (gs == 7) { // GAME_STATE_FRONTEND_IDLE
        s_forced = true;
        // Force menu to stop and trigger new game
        // FrontEndMenuManager.m_bMenuActive = false (offset 0xBA6748 + 0x60)
        // FrontEndMenuManager.m_bLoadingData = true
        // Or simpler: call CGame::FrontEndMenuManager_StartGame directly
        // Even simpler: just force gGameState to 8 (LOADING_STARTED)
        *(int*)0xC8D4C0 = 8; // GAME_STATE_LOADING_STARTED
        Win32AppendLog("C:\\gamestate.log", "Forced state 7->8 (skip menu, start new game)\r\n");
    }
}

// Render2dStuff replacement — calls soak test frame counter
static void __cdecl StubRender2dWithSoakTest(...) {
    // Keep ForegroundApp true every frame (WndProc may clear it on WM_KILLFOCUS)
    *(bool*)0x8D621C = true;
    static int s_r2dCount = 0;
    if (s_r2dCount < 5 || s_r2dCount % 500 == 0) {
        char buf[128];
        wsprintfA(buf, "[Render2d] call #%d state=%d frame=%u\r\n",
                  s_r2dCount, *(int*)0xC8D4C0, *(uint32_t*)0xB7CB4C);
        Win32AppendLog("C:\\stub_calls.log", buf);
    }
    s_r2dCount++;
    SoakTestOnFrame();
}

// psCameraShowRaster replacement — calls soak test then returns first arg
// This is called every frame regardless of game state
static void* __cdecl StubShowRasterWithSoakTest(void* camera, ...) {
    static int s_csrCount = 0;
    if (s_csrCount < 5 || s_csrCount % 500 == 0) {
        char buf[128];
        wsprintfA(buf, "[CamShowRaster] call #%d state=%d frame=%u\r\n",
                  s_csrCount, *(int*)0xC8D4C0, *(uint32_t*)0xB7CB4C);
        Win32AppendLog("C:\\stub_calls.log", buf);
    }
    s_csrCount++;
    SoakTestOnFrame();
    return camera;
}

// ---------------------------------------------------------------------------
// Patching helpers
// ---------------------------------------------------------------------------

void PatchJmp(uintptr_t target, uintptr_t stub) {
    DWORD oldProtect;
    VirtualProtect((void*)target, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    *(uint8_t*)target = 0xE9; // JMP rel32
    *(int32_t*)(target + 1) = (int32_t)(stub - target - 5);
    VirtualProtect((void*)target, 5, oldProtect, &oldProtect);
}

static void PatchDword(uintptr_t addr, uint32_t value) {
    DWORD oldProt;
    VirtualProtect((void*)addr, 4, PAGE_READWRITE, &oldProt);
    *(uint32_t*)addr = value;
    VirtualProtect((void*)addr, 4, oldProt, &oldProt);
}

// ---------------------------------------------------------------------------
// IAT hook — replace an imported function in gta_sa.exe's import table
// ---------------------------------------------------------------------------

static void HookIAT(HMODULE hModule, const char* dllName, const char* funcName, void* newFunc) {
    auto* dos = (IMAGE_DOS_HEADER*)hModule;
    auto* nt  = (IMAGE_NT_HEADERS*)((uintptr_t)hModule + dos->e_lfanew);
    auto& importDir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (!importDir.VirtualAddress) return;

    auto* desc = (IMAGE_IMPORT_DESCRIPTOR*)((uintptr_t)hModule + importDir.VirtualAddress);
    for (; desc->Name; desc++) {
        auto* name = (const char*)((uintptr_t)hModule + desc->Name);
        if (_stricmp(name, dllName) != 0) continue;

        auto* thunk = (IMAGE_THUNK_DATA*)((uintptr_t)hModule + desc->FirstThunk);
        auto* origThunk = (IMAGE_THUNK_DATA*)((uintptr_t)hModule + desc->OriginalFirstThunk);
        for (; origThunk->u1.AddressOfData; origThunk++, thunk++) {
            if (origThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) continue;
            auto* import = (IMAGE_IMPORT_BY_NAME*)((uintptr_t)hModule + origThunk->u1.AddressOfData);
            if (strcmp((const char*)import->Name, funcName) == 0) {
                DWORD oldProt;
                VirtualProtect(&thunk->u1.Function, sizeof(void*), PAGE_READWRITE, &oldProt);
                thunk->u1.Function = (uintptr_t)newFunc;
                VirtualProtect(&thunk->u1.Function, sizeof(void*), oldProt, &oldProt);
                return;
            }
        }
    }
}

// Stub DialogBoxParam — auto-returns 1 (IDOK) so dialogs don't block headless mode
static INT_PTR WINAPI StubDialogBoxParamA(HINSTANCE, LPCSTR lpTemplateName, HWND, DLGPROC, LPARAM) {
    InitLog("StubDialogBoxParamA called, template=%p", (void*)lpTemplateName);
    return 1; // IDOK
}
static INT_PTR WINAPI StubDialogBoxParamW(HINSTANCE, LPCWSTR lpTemplateName, HWND, DLGPROC, LPARAM) {
    InitLog("StubDialogBoxParamW called, template=%p", (void*)lpTemplateName);
    return 1; // IDOK
}

// Stub MessageBox — auto-returns IDOK
static int WINAPI StubMessageBoxA(HWND, LPCSTR lpText, LPCSTR lpCaption, UINT) {
    InitLog("StubMessageBoxA: caption='%s' text='%s'",
            lpCaption ? lpCaption : "(null)", lpText ? lpText : "(null)");
    return IDOK;
}
static int WINAPI StubMessageBoxW(HWND, LPCWSTR lpText, LPCWSTR lpCaption, UINT) {
    InitLog("StubMessageBoxW called");
    return IDOK;
}

// Hooked Sleep — when the game calls Sleep(100) in its foreground check,
// force ForegroundApp=TRUE so next iteration falls through to game logic.
static decltype(&Sleep) OriginalSleep = &Sleep;
static void WINAPI HookedSleep(DWORD dwMs) {
    static int s_callCount = 0;
    s_callCount++;
    if (s_callCount <= 5 || s_callCount % 200 == 0) {
        InitLog("HookedSleep(%lu) call #%d ForegroundApp=%d gGameState=%d",
                dwMs, s_callCount, *(int*)0x8D621C, *(int*)0xC8D4C0);
    }
    if (dwMs == 100) {
        *(bool*)0x8D621C = true;
        return;
    }
    OriginalSleep(dwMs);
}

// ---------------------------------------------------------------------------
// Logged RsRwInitialize — replaces the ASI's RsRwInitialize
// Calls RW functions at their game addresses with logging around each step.
// ---------------------------------------------------------------------------

// Function pointer types for raw game/RW functions
typedef int  (__cdecl *fn_RwBool_void)();
typedef int  (__cdecl *fn_RwEngineInit_t)(void* memFuncs, uint32_t flags, uint32_t arenaSize);
typedef int  (__cdecl *fn_RwEngineOpen_t)(void* openParams);
typedef int  (__cdecl *fn_RwEngineGetNumSS)();
typedef int  (__cdecl *fn_RwEngineGetCurSS)();
typedef int  (__cdecl *fn_RwEngineSetSS)(int idx);
typedef int  (__cdecl *fn_RwEngineSetVM)(int idx);
typedef void*(__cdecl *fn_RwEngineGetSSInfo)(void* info, int idx);
typedef void*(__cdecl *fn_RwEngineGetVMInfo)(void* vmi, int idx);
typedef int  (__cdecl *fn_RwEngineGetNumVM)();
typedef void*(__cdecl *fn_psGetMemFuncs)();
typedef int  (__cdecl *fn_psInstallFS)();

// Game addresses for raw RW functions (NOT hooked by gta-reversed)
static auto const pRwEngineInit       = (fn_RwEngineInit_t)0x7F3170;
static auto const pRwEngineOpen       = (fn_RwEngineOpen_t)0x7F2F70;
static auto const pRwEngineStart      = (fn_RwBool_void)0x7F2E70;
static auto const pRwEngineClose      = (fn_RwBool_void)0x7F2F00;
static auto const pRwEngineTerm       = (fn_RwBool_void)0x7F3130;
static auto const pRwEngineGetNumSS   = (fn_RwEngineGetNumSS)0x7F2C00;
static auto const pRwEngineGetCurSS   = (fn_RwEngineGetCurSS)0x7F2C60;
static auto const pRwEngineSetSS      = (fn_RwEngineSetSS)0x7F2C90;
static auto const pRwEngineGetSSInfo  = (fn_RwEngineGetSSInfo)0x7F2C30;
static auto const pRwEngineGetNumVM   = (fn_RwEngineGetNumVM)0x7F2CC0;
static auto const pRwEngineSetVM      = (fn_RwEngineSetVM)0x7F2D50;
static auto const pRwEngineGetVMInfo  = (fn_RwEngineGetVMInfo)0x7F2CF0;
static auto const pPsGetMemFuncs     = (fn_psGetMemFuncs)0x745510;
static auto const pPsInstallFS       = (fn_psInstallFS)0x745520;

// Game address for AppEventHandler (may run original or reversed code depending on hook state)
typedef int (__cdecl *fn_AppEventHandler)(int event, void* param);
static auto const pAppEventHandler = (fn_AppEventHandler)0x53EC10;

// RsEvent enum values (from skeleton.h)
enum {
    kRsINITDEBUG           = 3,
    kRsINPUTDEVICEATTACH   = 4,
    kRsPLUGINATTACH         = 9,
    kRsREGISTERIMAGELOADER = 10,
    kRsSELECTDEVICE         = 23,
};

// RsEventStatus enum values
enum {
    kRsEVENTERROR          = 0,
    kRsEVENTPROCESSED      = 1,
    kRsEVENTNOTPROCESSED   = 2,
};

// Game globals
static auto& gUseDefaultVM       = *(int*)0xC920FC;
static auto& gGnumSubSystems     = *(int*)0xC920F0;
static auto& gGcurSelSS          = *(int*)0xC920F4;
static auto& gGcurSelVM          = *(int*)0x8D6220;
static auto& gMultipleSubSystems = *(int*)0xC92118;

static bool LoggedPsSelectDevice() {
    InitLog("=== LoggedPsSelectDevice() ===");
    InitLog("  UseDefaultVM=%d GcurSelSS=%d GcurSelVM=%d", gUseDefaultVM, gGcurSelSS, gGcurSelVM);

    // Enumerate subsystems
    int numSS = pRwEngineGetNumSS();
    InitLog("  RwEngineGetNumSubSystems() = %d", numSS);
    if (numSS <= 0) {
        InitLog("  ERROR: No subsystems available — D3D9 backend not initialized?");
        // Check if d3d9.dll is loaded
        HMODULE hD3D9 = GetModuleHandleA("d3d9.dll");
        InitLog("  d3d9.dll module handle = %p", (void*)hD3D9);
        if (hD3D9) {
            auto pDC9 = (void*)GetProcAddress(hD3D9, "Direct3DCreate9");
            InitLog("  Direct3DCreate9 proc = %p", pDC9);
        }
        return false;
    }

    gGnumSubSystems = numSS;

    // Get subsystem info for logging
    struct { char name[80]; } ssInfo;
    for (int i = 0; i < numSS && i < 4; i++) {
        memset(&ssInfo, 0, sizeof(ssInfo));
        pRwEngineGetSSInfo(&ssInfo, i);
        InitLog("  Subsystem[%d]: '%s'", i, ssInfo.name);
    }

    // Get current subsystem
    gGcurSelSS = pRwEngineGetCurSS();
    InitLog("  RwEngineGetCurrentSubSystem() = %d", gGcurSelSS);

    gMultipleSubSystems = (numSS > 1) ? 1 : 0;
    InitLog("  MultipleSubSystems=%d", gMultipleSubSystems);

    // Skip dialog — headless mode, just use defaults
    InitLog("  Skipping dialog (headless mode)");

    // Set subsystem
    InitLog("  Calling RwEngineSetSubSystem(%d)...", gGcurSelSS);
    if (!pRwEngineSetSS(gGcurSelSS)) {
        InitLog("  RwEngineSetSubSystem FAILED");
        return false;
    }
    InitLog("  RwEngineSetSubSystem OK");

    // Enumerate video modes
    int numVM = pRwEngineGetNumVM();
    InitLog("  RwEngineGetNumVideoModes() = %d", numVM);

    // Log first few video modes
    struct { int width; int height; int depth; int flags; int refreshRate; int format; } vmi;
    for (int i = 0; i < numVM && i < 8; i++) {
        memset(&vmi, 0, sizeof(vmi));
        pRwEngineGetVMInfo(&vmi, i);
        InitLog("  VM[%d]: %dx%d depth=%d flags=0x%X refresh=%d",
                i, vmi.width, vmi.height, vmi.depth, vmi.flags, vmi.refreshRate);
    }

    // Use video mode 0 (windowed mode, usually default)
    gGcurSelVM = 0;
    InitLog("  Calling RwEngineSetVideoMode(%d)...", gGcurSelVM);
    if (!pRwEngineSetVM(gGcurSelVM)) {
        InitLog("  RwEngineSetVideoMode FAILED");
        return false;
    }
    InitLog("  RwEngineSetVideoMode OK");

    InitLog("=== LoggedPsSelectDevice() returning TRUE ===");
    return true;
}

// Forward declaration
static DWORD WINAPI DebugMonitorThread(LPVOID);

static bool LoggedRsRwInitialize(void* param) {
    InitLog("=== LoggedRsRwInitialize(hwnd=%p) ===", param);

    // Check if d3d9.dll is loaded before we start
    HMODULE hD3D9 = GetModuleHandleA("d3d9.dll");
    InitLog("  d3d9.dll loaded at start: %p", (void*)hD3D9);

    // Step 1: RwEngineInit
    void* memFuncs = pPsGetMemFuncs();
    InitLog("  psGetMemoryFunctions() = %p", memFuncs);

    uint32_t arenaSize = 4 << 20; // 4MB for D3D9 platform
    InitLog("  Calling RwEngineInit(%p, 0, 0x%X)...", memFuncs, arenaSize);
    if (!pRwEngineInit(memFuncs, 0, arenaSize)) {
        InitLog("  RwEngineInit FAILED!");
        return false;
    }
    InitLog("  RwEngineInit OK");

    // Step 2: rsINITDEBUG event
    InitLog("  Calling AppEventHandler(rsINITDEBUG)...");
    pAppEventHandler(kRsINITDEBUG, nullptr);
    InitLog("  rsINITDEBUG OK");

    // Step 3: psInstallFileSystem
    InitLog("  Calling psInstallFileSystem()...");
    pPsInstallFS();
    InitLog("  psInstallFileSystem OK");

    // Step 4: rsPLUGINATTACH event (PluginAttach — attaches RW plugins)
    InitLog("  Calling AppEventHandler(rsPLUGINATTACH)...");
    int paResult = pAppEventHandler(kRsPLUGINATTACH, nullptr);
    InitLog("  rsPLUGINATTACH result=%d (0=error, 1=processed, 2=notprocessed)", paResult);
    if (paResult == kRsEVENTERROR) {
        InitLog("  PluginAttach FAILED!");
        return false;
    }

    // Step 5: rsINPUTDEVICEATTACH event
    InitLog("  Calling AppEventHandler(rsINPUTDEVICEATTACH)...");
    int idResult = pAppEventHandler(kRsINPUTDEVICEATTACH, nullptr);
    InitLog("  rsINPUTDEVICEATTACH result=%d", idResult);
    if (idResult == kRsEVENTERROR) {
        InitLog("  InputDeviceAttach FAILED!");
        return false;
    }

    // Step 6: RwEngineOpen — THIS should call Direct3DCreate9
    hD3D9 = GetModuleHandleA("d3d9.dll");
    InitLog("  d3d9.dll before RwEngineOpen: %p", (void*)hD3D9);

    struct { void* displayID; } openParams = { param };
    InitLog("  Calling RwEngineOpen(displayID=%p)...", param);
    if (!pRwEngineOpen(&openParams)) {
        InitLog("  RwEngineOpen FAILED!");
        // Check d3d9 state after failure
        hD3D9 = GetModuleHandleA("d3d9.dll");
        InitLog("  d3d9.dll after failed RwEngineOpen: %p", (void*)hD3D9);
        // Try to get last error
        InitLog("  GetLastError()=%lu", GetLastError());
        pRwEngineTerm();
        return false;
    }
    InitLog("  RwEngineOpen OK");

    // Check d3d9 after successful open
    hD3D9 = GetModuleHandleA("d3d9.dll");
    InitLog("  d3d9.dll after RwEngineOpen: %p", (void*)hD3D9);
    if (hD3D9) {
        auto pDC9 = (void*)GetProcAddress(hD3D9, "Direct3DCreate9");
        InitLog("  Direct3DCreate9 proc at %p", pDC9);
    }

    // Step 7: psSelectDevice
    InitLog("  Calling LoggedPsSelectDevice()...");
    if (!LoggedPsSelectDevice()) {
        InitLog("  psSelectDevice FAILED!");
        pRwEngineClose();
        pRwEngineTerm();
        return false;
    }

    // Step 8: RwEngineStart
    // Install render stubs BEFORE RwEngineStart to prevent it from blocking.
    // RwEngineStart at 0x7F2E70 calls CreateDevice then hangs — possibly doing
    // swap chain setup or calling a blocking Win32 function. Install camera stubs
    // so any internal RW calls during startup are no-ops.
    InitLog("  Installing render stubs (pre-RwEngineStart)...");
    PatchJmp(0x53DF40, (uintptr_t)StubVoidNoOp);       // RenderScene
    PatchJmp(0x53E170, (uintptr_t)StubVoidNoOp);       // RenderEffects
    PatchJmp(0x53E230, (uintptr_t)StubRender2dWithSoakTest); // Render2dStuff
    PatchJmp(0x7EE340, (uintptr_t)StubReturnFirstArg); // RwCameraClear
    PatchJmp(0x7EE190, (uintptr_t)StubReturnFirstArg); // RwCameraBeginUpdate
    PatchJmp(0x7EE180, (uintptr_t)StubReturnFirstArg); // RwCameraEndUpdate
    PatchJmp(0x7EE370, (uintptr_t)StubReturnFirstArg); // RwCameraShowRaster
    PatchJmp(0x619440, (uintptr_t)StubReturnFirstArg); // RsCameraShowRaster
    PatchJmp(0x619450, (uintptr_t)StubReturnTrue);     // RsCameraBeginUpdate
    PatchJmp(0x745240, (uintptr_t)StubShowRasterWithSoakTest); // psCameraShowRaster
    InitLog("  Render stubs installed");

    InitLog("  Calling RwEngineStart()...");
    if (!pRwEngineStart()) {
        InitLog("  RwEngineStart FAILED!");
        pRwEngineClose();
        pRwEngineTerm();
        return false;
    }
    InitLog("  RwEngineStart OK");

    // Now install render stubs — RW is fully initialized
    InitLog("  Installing render stubs (post-RwEngineStart)...");
    PatchJmp(0x53DF40, (uintptr_t)StubVoidNoOp);       // RenderScene (game addr)
    PatchJmp(0x53E170, (uintptr_t)StubVoidNoOp);       // RenderEffects (game addr)
    PatchJmp(0x53E230, (uintptr_t)StubRender2dWithSoakTest); // Render2dStuff (game addr)

    // ALSO patch ASI's own functions — locked hooks (ProcessGameLogic, Idle)
    // call these as intra-DLL calls that bypass game-address PatchJmps.
    {
        extern void RenderScene();
        extern void RenderEffects();
        extern void Render2dStuff();
        PatchJmp((uintptr_t)&RenderScene, (uintptr_t)StubVoidNoOp);
        PatchJmp((uintptr_t)&RenderEffects, (uintptr_t)StubVoidNoOp);
        PatchJmp((uintptr_t)&Render2dStuff, (uintptr_t)StubRender2dWithSoakTest);
        InitLog("  ASI intra-DLL render stubs installed");
    }
    PatchJmp(0x7EE340, (uintptr_t)StubReturnFirstArg); // RwCameraClear
    PatchJmp(0x7EE190, (uintptr_t)StubReturnFirstArg); // RwCameraBeginUpdate
    PatchJmp(0x7EE180, (uintptr_t)StubReturnFirstArg); // RwCameraEndUpdate
    PatchJmp(0x7EE370, (uintptr_t)StubReturnFirstArg); // RwCameraShowRaster
    PatchJmp(0x619440, (uintptr_t)StubReturnFirstArg); // RsCameraShowRaster
    PatchJmp(0x619450, (uintptr_t)StubReturnTrue);     // RsCameraBeginUpdate
    PatchJmp(0x745240, (uintptr_t)StubShowRasterWithSoakTest); // psCameraShowRaster

    // Patch ASI versions of camera functions too
    {
        extern RwCamera* RwCameraClear(RwCamera*, RwRGBA*, RwInt32);
        extern RwCamera* RwCameraBeginUpdate(RwCamera*);
        extern RwCamera* RwCameraEndUpdate(RwCamera*);
        extern RwCamera* RwCameraShowRaster(RwCamera*, void*, RwUInt32);
        PatchJmp((uintptr_t)&RwCameraClear, (uintptr_t)StubReturnFirstArg);
        PatchJmp((uintptr_t)&RwCameraBeginUpdate, (uintptr_t)StubReturnFirstArg);
        PatchJmp((uintptr_t)&RwCameraEndUpdate, (uintptr_t)StubReturnFirstArg);
        PatchJmp((uintptr_t)&RwCameraShowRaster, (uintptr_t)StubReturnFirstArg);
        InitLog("  ASI camera stubs installed");
    }
    InitLog("  Render stubs installed (post-RwEngineStart)");

    // Step 9: rsREGISTERIMAGELOADER event
    InitLog("  Calling AppEventHandler(rsREGISTERIMAGELOADER)...");
    pAppEventHandler(kRsREGISTERIMAGELOADER, nullptr);
    InitLog("  rsREGISTERIMAGELOADER OK");

    // Step 10: psNativeTextureSupport
    typedef void (__cdecl *fn_void)();
    auto pPsNativeTexSupport = (fn_void)0x745530;
    InitLog("  Calling psNativeTextureSupport()...");
    pPsNativeTexSupport();
    InitLog("  psNativeTextureSupport OK");

    // Step 11: RwTextureSetMipmapping / RwTextureSetAutoMipmapping
    typedef int (__cdecl *fn_RwTexSetMip)(int);
    auto pRwTexSetMip = (fn_RwTexSetMip)0x7F3530; // RwTextureSetMipmapping
    auto pRwTexSetAutoMip = (fn_RwTexSetMip)0x7F3560; // RwTextureSetAutoMipmapping
    pRwTexSetMip(1);
    pRwTexSetAutoMip(0);
    InitLog("  RwTextureSetMipmapping(TRUE), RwTextureSetAutoMipmapping(FALSE)");

    InitLog("=== LoggedRsRwInitialize() returning TRUE ===");
    return true;
}

// Logged RwInitialize — replaces app_game.cpp's RwInitialize
// which calls RsRwInitialize + CGame::InitialiseRenderWare
static bool LoggedRwInitialize(void* param) {
    InitLog("=== LoggedRwInitialize(param=%p) ===", param);

    if (!LoggedRsRwInitialize(param)) {
        InitLog("  RsRwInitialize part FAILED");
        return false;
    }

    // CGame::InitialiseRenderWare at 0x5BD600
    typedef int (__cdecl *fn_InitRW)();
    auto pInitRW = (fn_InitRW)0x5BD600;
    InitLog("  Calling CGame::InitialiseRenderWare()...");
    if (!pInitRW()) {
        InitLog("  CGame::InitialiseRenderWare FAILED!");
        return false;
    }
    InitLog("  CGame::InitialiseRenderWare OK");

    InitLog("=== LoggedRwInitialize() returning TRUE ===");

    // Post-init: start state monitor thread from HERE (not DllMain).
    // Creating threads during DllMain is unsafe (loader lock). Creating here
    // is after all init is done and the loader lock is released.
    Win32AppendLog("C:\\gamestate.log", "=== Post-init state monitor ===\r\n");
    {
        char buf[256];
        wsprintfA(buf, "init_done state=%d rwInit=%d frame=%u\r\n",
                  *(int*)0xC8D4C0, *(int*)0xC920E8, *(uint32_t*)0xB7CB4C);
        Win32AppendLog("C:\\gamestate.log", buf);
    }

    HANDLE hThread = CreateThread(NULL, 0, DebugMonitorThread, NULL, 0, NULL);
    if (hThread) {
        Win32AppendLog("C:\\gamestate.log", "Monitor thread created from LoggedRwInitialize\r\n");
        CloseHandle(hThread);
    } else {
        char buf[128];
        wsprintfA(buf, "CreateThread FAILED err=%lu\r\n", GetLastError());
        Win32AppendLog("C:\\gamestate.log", buf);
    }

    return true;
}

// ---------------------------------------------------------------------------
// Debug monitor — background thread that logs gGameState periodically.
// Uses a thread instead of SetTimer because SDL3's event pump doesn't
// dispatch Win32 timer messages.
// ---------------------------------------------------------------------------

static DWORD WINAPI DebugMonitorThread(LPVOID) {
    int tick = 0;
    int lastState = -1;
    int stuckCount = 0;

    Sleep(2000); // Wait longer for init to complete
    Win32AppendLog("C:\\gamestate.log", "=== Debug Monitor Thread Started (2s delay) ===\r\n");

    while (true) {
        Sleep(100);
        tick++;

        int gs = *(int*)0xC8D4C0;
        int fg = *(int*)0x8D621C;
        int rwInit = *(int*)0xC920E8;
        uint32_t frame = *(uint32_t*)0xB7CB4C;

        char buf[256];
        wsprintfA(buf, "tick=%d state=%d fg=%d rwInit=%d frame=%u\r\n",
                  tick, gs, fg, rwInit, frame);
        Win32AppendLog("C:\\gamestate.log", buf);

        if (gs == lastState) {
            stuckCount++;
        } else {
            stuckCount = 0;
        }
        lastState = gs;

        // Force past splash/video states (1-4)
        if (stuckCount >= 10 && gs >= 1 && gs <= 4) {
            wsprintfA(buf, "  FORCING state %d -> 5 (stuck %d ticks)\r\n", gs, stuckCount);
            Win32AppendLog("C:\\gamestate.log", buf);
            *(int*)0xC8D4C0 = 5;
            stuckCount = 0;
        }

        // Re-apply CLoadingScreen patches once
        static bool s_patched = false;
        if (!s_patched && tick == 5) {
            s_patched = true;
            extern void PatchJmp(uintptr_t target, uintptr_t stub);
            PatchJmp(0x590570, (uintptr_t)StubVoidNoOp);
            PatchJmp(0x590860, (uintptr_t)StubVoidNoOp);
            PatchJmp(0x590990, (uintptr_t)StubVoidNoOp);
            PatchJmp(0x590AC0, (uintptr_t)StubVoidNoOp);
            PatchJmp(0x58FF60, (uintptr_t)StubVoidNoOp);
            PatchJmp(0x53DED0, (uintptr_t)StubVoidNoOp);
            Win32AppendLog("C:\\gamestate.log", "  Re-applied CLoadingScreen patches\r\n");
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Public entry point — called from DllMain
// ---------------------------------------------------------------------------

void InstallHeadlessRenderStubs() {
    // Immediate proof-of-life log — use absolute path since CWD may vary
    {
        FILE* f = fopen("C:\\headless_stubs.log", "w");
        if (f) { fprintf(f, "InstallHeadlessRenderStubs() called\n"); fclose(f); }
    }
    // Init the rw_init log fresh
    {
        FILE* f = fopen("C:\\rw_init.log", "w");
        if (f) { fprintf(f, "=== RW Init Log ===\n"); fclose(f); }
    }

    InitLog("InstallHeadlessRenderStubs starting...");

    // --- Check d3d9.dll status ---
    HMODULE hD3D9 = GetModuleHandleA("d3d9.dll");
    InitLog("d3d9.dll at DllMain time: %p", (void*)hD3D9);
    if (hD3D9) {
        char path[MAX_PATH];
        GetModuleFileNameA(hD3D9, path, sizeof(path));
        InitLog("d3d9.dll path: %s", path);
        auto pDC9 = GetProcAddress(hD3D9, "Direct3DCreate9");
        InitLog("Direct3DCreate9 export: %p", (void*)pDC9);
    }

    // --- Skip all dialogs ---
    // Hook both A and W versions in both the game exe and our ASI DLL.
    // The reversed code (compiled with UNICODE) calls DialogBoxParamW via the ASI's IAT.
    // The original game code calls DialogBoxParamA via the exe's IAT.
    HMODULE hExe = GetModuleHandleA(nullptr);
    HMODULE hAsi = nullptr;
    GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCSTR)&InstallHeadlessRenderStubs, &hAsi);

    // Hook exe IAT
    HookIAT(hExe, "USER32.dll", "DialogBoxParamA", (void*)StubDialogBoxParamA);
    HookIAT(hExe, "USER32.dll", "DialogBoxParamW", (void*)StubDialogBoxParamW);
    HookIAT(hExe, "USER32.dll", "MessageBoxA",     (void*)StubMessageBoxA);
    HookIAT(hExe, "USER32.dll", "MessageBoxW",     (void*)StubMessageBoxW);

    // Hook ASI IAT (the reversed code calls W versions via the DLL's own imports)
    if (hAsi && hAsi != hExe) {
        HookIAT(hAsi, "USER32.dll", "DialogBoxParamA", (void*)StubDialogBoxParamA);
        HookIAT(hAsi, "USER32.dll", "DialogBoxParamW", (void*)StubDialogBoxParamW);
        HookIAT(hAsi, "USER32.dll", "MessageBoxA",     (void*)StubMessageBoxA);
        HookIAT(hAsi, "USER32.dll", "MessageBoxW",     (void*)StubMessageBoxW);
        InitLog("IAT hooks installed on both exe (%p) and ASI (%p)", (void*)hExe, (void*)hAsi);
    } else {
        InitLog("IAT hooks installed on exe (%p) only (ASI=%p)", (void*)hExe, (void*)hAsi);
    }

    // Skip the video mode dialog in the ORIGINAL psSelectDevice at game address
    // (in case original code path is used instead of our logged replacement)
    {
        DWORD oldProt;
        VirtualProtect((void*)0x74629B, 1, PAGE_EXECUTE_READWRITE, &oldProt);
        *(uint8_t*)0x74629B = 0xEB; // JNZ -> JMP
        VirtualProtect((void*)0x74629B, 1, oldProt, &oldProt);
    }

    // Force past the ForegroundApp check (game address — for original code path)
    {
        DWORD oldProt;
        VirtualProtect((void*)0x748A8D, 6, PAGE_EXECUTE_READWRITE, &oldProt);
        memset((void*)0x748A8D, 0x90, 6); // NOP x6
        VirtualProtect((void*)0x748A8D, 6, oldProt, &oldProt);
    }
    // Force ForegroundApp=true (at 0x8D621C) — the reversed ProcessGameLogic
    // (called from locked NOTSA_WinMain) checks this variable directly. Under
    // headless Wine the window never gains focus, so ForegroundApp stays false
    // and the game loop sleeps instead of calling Idle/Render2dStuff.
    *(bool*)0x8D621C = true;
    InitLog("ForegroundApp forced TRUE at 0x8D621C");

    // --- Redirect RsRwInitialize and psSelectDevice in the ASI ---
    // The reversed WinMain (locked hook) calls these internally within the ASI DLL.
    // PatchJmp replaces the first 5 bytes of the ASI's own functions with JMP to
    // our logged versions. The logged versions call RW functions at their game
    // addresses (0x7F3170 etc.) which are NOT hooked.
    //
    // Call chain:  NOTSA_WinMain (locked, always reversed)
    //   -> RsEventHandler -> AppEventHandler -> RwInitialize -> RsRwInitialize
    //   All of these are C++ function calls WITHIN the ASI DLL.
    //   By patching RwInitialize (which calls RsRwInitialize + CGame::InitialiseRenderWare),
    //   we intercept the entire init sequence.

    // Patch the ASI's own RwInitialize function (app_game.cpp, 0x5BF390 game addr)
    // We get the ASI's function address via extern declaration
    {
        // RwInitialize is defined in app_game.cpp:
        //   bool RwInitialize(void* param) { return RsRwInitialize(param) && CGame::InitialiseRenderWare(); }
        extern bool RwInitialize(void* param);
        uintptr_t rwInitAddr = (uintptr_t)&RwInitialize;
        InitLog("Patching ASI's RwInitialize at %p -> LoggedRwInitialize at %p",
                (void*)rwInitAddr, (void*)&LoggedRwInitialize);
        PatchJmp(rwInitAddr, (uintptr_t)&LoggedRwInitialize);
    }

    // Also patch the GAME address for psSelectDevice (0x746190) in case it's called
    // via the game's code path (hook disabled)
    InitLog("Patching game psSelectDevice at 0x746190 -> LoggedPsSelectDevice");
    PatchJmp(0x746190, (uintptr_t)&LoggedPsSelectDevice);

    // --- Skip all CLoadingScreen display functions ---
    PatchJmp(0x590570, (uintptr_t)StubVoidNoOp);   // DisplayPCScreen
    PatchJmp(0x590860, (uintptr_t)StubVoidNoOp);   // DoPCTitleFadeOut
    PatchJmp(0x590990, (uintptr_t)StubVoidNoOp);   // DoPCTitleFadeIn
    PatchJmp(0x590AC0, (uintptr_t)StubVoidNoOp);   // DoPCScreenChange
    PatchJmp(0x58FF60, (uintptr_t)StubVoidNoOp);   // RenderSplash
    PatchJmp(0x53DED0, (uintptr_t)StubVoidNoOp);   // LoadingScreen
    InitLog("CLoadingScreen stubs installed");

    // Marker for MainLoop detection
    PatchDword(0xC8D4C4, 0xAA);

    // Test Win32AppendLog works at all
    Win32AppendLog("C:\\win32log_test.log", "Win32AppendLog works from DllMain!\r\n");
    InitLog("Win32AppendLog test written");

    // Debug monitor thread is created from LoggedRwInitialize (after init, not DllMain)
    InitLog("Monitor thread will be created after RW init completes");

    // Render stubs are now installed AFTER RwEngineStart in LoggedRsRwInitialize.
    // Installing them early breaks RwEngineStart which calls camera functions internally.
    InitLog("Render stubs will be installed after RwEngineStart (in LoggedRsRwInitialize)");

    InitLog("InstallHeadlessRenderStubs complete — waiting for WinMain to call RwInitialize");
}
