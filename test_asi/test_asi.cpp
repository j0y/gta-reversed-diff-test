// Minimal test ASI — verifies DLL loading mechanism works under Wine.
// No static constructors, no game headers, no dependencies.
// Just writes a log file from DllMain and optionally from a timer callback.

#include <windows.h>
#include <cstdio>

// Write a timestamped message to the log file
static void Log(const char* msg) {
    FILE* f = fopen("C:\\test_asi.log", "a");
    if (f) {
        DWORD tick = GetTickCount();
        fprintf(f, "[%08lu] %s\n", tick, msg);
        fclose(f);
    }
}

// Timer callback — proves the game loop is running and our DLL is alive
static void CALLBACK TimerProc(HWND, UINT, UINT_PTR, DWORD) {
    static int count = 0;
    char buf[128];
    _snprintf(buf, sizeof(buf), "TimerProc fired #%d", ++count);
    Log(buf);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    switch (reason) {
    case DLL_PROCESS_ATTACH: {
        // Delete old log
        DeleteFileA("C:\\test_asi.log");

        Log("=== test_asi.dll DllMain DLL_PROCESS_ATTACH ===");

        // Log basic info
        char buf[256];
        _snprintf(buf, sizeof(buf), "hModule=0x%08X lpReserved=0x%08X",
                  (unsigned)hModule, (unsigned)lpReserved);
        Log(buf);

        // Test: can we call GetModuleHandle?
        HMODULE hKernel = GetModuleHandleA("kernel32.dll");
        _snprintf(buf, sizeof(buf), "kernel32.dll at 0x%08X", (unsigned)hKernel);
        Log(buf);

        // Test: can we call GetCommandLineA?
        const char* cmdline = GetCommandLineA();
        _snprintf(buf, sizeof(buf), "CommandLine: %.200s", cmdline ? cmdline : "(null)");
        Log(buf);

        // Test: __argc/__argv (MSVC CRT globals)
        extern int __argc;
        extern char** __argv;
        _snprintf(buf, sizeof(buf), "__argc=%d __argv=0x%08X",
                  __argc, (unsigned)__argv);
        Log(buf);
        for (int i = 0; i < __argc && i < 5; i++) {
            _snprintf(buf, sizeof(buf), "  argv[%d]='%.100s'", i,
                      __argv[i] ? __argv[i] : "(null)");
            Log(buf);
        }

        // Set a repeating timer (fires every 2 seconds)
        SetTimer(NULL, 0, 2000, TimerProc);

        Log("DllMain DLL_PROCESS_ATTACH complete");
        break;
    }
    case DLL_PROCESS_DETACH:
        Log("DllMain DLL_PROCESS_DETACH");
        break;
    }
    return TRUE;
}
