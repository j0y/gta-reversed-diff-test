// TestFramework.h — Lightweight in-process test framework for gta-reversed.
//
// Tests run inside the game process at state 9 with full access to game state.
// Triggered by GAME_TEST_ENABLE=1 environment variable.
//
// Usage:
//   GAME_TEST(CVector, Normalise_UnitLength) {
//       CVector v(3.0f, 4.0f, 0.0f);
//       v.Normalise();
//       EXPECT_NEAR(v.Magnitude(), 1.0f, 1e-5f);
//   }

#pragma once

#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>
#include <tlhelp32.h>

// ---------------------------------------------------------------------------
// Test registry
// ---------------------------------------------------------------------------

struct GameTestEntry {
    const char* className;
    const char* testName;
    void (*fn)();
};

struct GameTestRegistry {
    static std::vector<GameTestEntry>& Tests() {
        static std::vector<GameTestEntry> tests;
        return tests;
    }
};

struct GameTestRegistrar {
    GameTestRegistrar(const char* cls, const char* name, void (*fn)()) {
        GameTestRegistry::Tests().push_back({ cls, name, fn });
    }
};

// ---------------------------------------------------------------------------
// Test macros
// ---------------------------------------------------------------------------

#define GAME_TEST(Class, Name) \
    static void GameTest_##Class##_##Name(); \
    static GameTestRegistrar g_reg_##Class##_##Name(#Class, #Name, GameTest_##Class##_##Name); \
    static void GameTest_##Class##_##Name()

// ---------------------------------------------------------------------------
// Test result tracking (per-test)
// ---------------------------------------------------------------------------

struct GameTestContext {
    const char* className;
    const char* testName;
    int assertions;
    int failures;
    char firstFailure[512];

    void Reset(const char* cls, const char* name) {
        className = cls;
        testName = name;
        assertions = 0;
        failures = 0;
        firstFailure[0] = '\0';
    }

    void RecordFailure(const char* file, int line, const char* msg) {
        failures++;
        if (firstFailure[0] == '\0') {
            _snprintf(firstFailure, sizeof(firstFailure), "%s:%d: %s", file, line, msg);
            firstFailure[sizeof(firstFailure) - 1] = '\0';
        }
    }
};

// Global context — set by the test runner before each test
inline GameTestContext& GetTestContext() {
    static GameTestContext ctx{};
    return ctx;
}

// ---------------------------------------------------------------------------
// Assertion macros
// ---------------------------------------------------------------------------

#define EXPECT_TRUE(expr) do { \
    GetTestContext().assertions++; \
    if (!(expr)) { \
        char _msg[256]; \
        _snprintf(_msg, sizeof(_msg), "EXPECT_TRUE(%s) failed", #expr); \
        GetTestContext().RecordFailure(__FILE__, __LINE__, _msg); \
    } \
} while(0)

#define EXPECT_FALSE(expr) do { \
    GetTestContext().assertions++; \
    if ((expr)) { \
        char _msg[256]; \
        _snprintf(_msg, sizeof(_msg), "EXPECT_FALSE(%s) failed", #expr); \
        GetTestContext().RecordFailure(__FILE__, __LINE__, _msg); \
    } \
} while(0)

#define EXPECT_EQ(a, b) do { \
    GetTestContext().assertions++; \
    if ((a) != (b)) { \
        char _msg[256]; \
        _snprintf(_msg, sizeof(_msg), "EXPECT_EQ(%s, %s) failed", #a, #b); \
        GetTestContext().RecordFailure(__FILE__, __LINE__, _msg); \
    } \
} while(0)

#define EXPECT_NE(a, b) do { \
    GetTestContext().assertions++; \
    if ((a) == (b)) { \
        char _msg[256]; \
        _snprintf(_msg, sizeof(_msg), "EXPECT_NE(%s, %s) failed", #a, #b); \
        GetTestContext().RecordFailure(__FILE__, __LINE__, _msg); \
    } \
} while(0)

#define EXPECT_LT(a, b) do { \
    GetTestContext().assertions++; \
    auto _a = (a); auto _b = (b); \
    if (!(_a < _b)) { \
        char _msg[256]; \
        _snprintf(_msg, sizeof(_msg), "EXPECT_LT(%s, %s) failed: %g >= %g", #a, #b, (double)_a, (double)_b); \
        GetTestContext().RecordFailure(__FILE__, __LINE__, _msg); \
    } \
} while(0)

#define EXPECT_GT(a, b) do { \
    GetTestContext().assertions++; \
    auto _a = (a); auto _b = (b); \
    if (!(_a > _b)) { \
        char _msg[256]; \
        _snprintf(_msg, sizeof(_msg), "EXPECT_GT(%s, %s) failed: %g <= %g", #a, #b, (double)_a, (double)_b); \
        GetTestContext().RecordFailure(__FILE__, __LINE__, _msg); \
    } \
} while(0)

#define EXPECT_NEAR(a, b, eps) do { \
    GetTestContext().assertions++; \
    auto _a = (a); auto _b = (b); auto _e = (eps); \
    if (std::fabs((double)_a - (double)_b) > (double)_e) { \
        char _msg[256]; \
        _snprintf(_msg, sizeof(_msg), "EXPECT_NEAR(%s, %s, %s) failed: |%g - %g| = %g > %g", \
            #a, #b, #eps, (double)_a, (double)_b, std::fabs((double)_a - (double)_b), (double)_e); \
        GetTestContext().RecordFailure(__FILE__, __LINE__, _msg); \
    } \
} while(0)

// Vector comparison helper
#define EXPECT_VEC_NEAR(vec, expected, eps) do { \
    EXPECT_NEAR((vec).x, (expected).x, eps); \
    EXPECT_NEAR((vec).y, (expected).y, eps); \
    EXPECT_NEAR((vec).z, (expected).z, eps); \
} while(0)

// ---------------------------------------------------------------------------
// Background thread suspension — ensures stable state during diff tests
// ---------------------------------------------------------------------------

// Suspend all threads in this process except the current one.
// Prevents CdStream worker and other background threads from modifying
// game state between original and reversed calls.
struct SuspendOtherThreads {
    std::vector<HANDLE> suspended;

    SuspendOtherThreads() {
        DWORD pid = GetCurrentProcessId();
        DWORD tid = GetCurrentThreadId();
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snap == INVALID_HANDLE_VALUE) return;

        THREADENTRY32 te{};
        te.dwSize = sizeof(te);
        if (Thread32First(snap, &te)) {
            do {
                if (te.th32OwnerProcessID == pid && te.th32ThreadID != tid) {
                    HANDLE h = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                    if (h) {
                        SuspendThread(h);
                        suspended.push_back(h);
                    }
                }
            } while (Thread32Next(snap, &te));
        }
        CloseHandle(snap);
    }

    ~SuspendOtherThreads() {
        for (HANDLE h : suspended) {
            ResumeThread(h);
            CloseHandle(h);
        }
    }

    SuspendOtherThreads(const SuspendOtherThreads&) = delete;
    SuspendOtherThreads& operator=(const SuspendOtherThreads&) = delete;
};

// Inverse of SuspendOtherThreads — resumes all threads so that CdStream I/O
// can complete. Re-suspends on destruction. Used by streaming tests that need
// the CdStream worker thread alive inside the SuspendOtherThreads scope.
struct ResumeOtherThreads {
    std::vector<HANDLE> threads;

    ResumeOtherThreads() {
        DWORD pid = GetCurrentProcessId();
        DWORD tid = GetCurrentThreadId();
        HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
        if (snap == INVALID_HANDLE_VALUE) return;
        THREADENTRY32 te{};
        te.dwSize = sizeof(te);
        if (Thread32First(snap, &te)) {
            do {
                if (te.th32OwnerProcessID == pid && te.th32ThreadID != tid) {
                    HANDLE h = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                    if (h) { ResumeThread(h); threads.push_back(h); }
                }
            } while (Thread32Next(snap, &te));
        }
        CloseHandle(snap);
    }

    ~ResumeOtherThreads() {
        for (HANDLE h : threads) { SuspendThread(h); CloseHandle(h); }
    }

    ResumeOtherThreads(const ResumeOtherThreads&) = delete;
    ResumeOtherThreads& operator=(const ResumeOtherThreads&) = delete;
};

// ---------------------------------------------------------------------------
// Differential test helpers — compare reversed vs original code
// ---------------------------------------------------------------------------

#include "reversiblehooks/ReversibleHooks.h"

// Toggle a single hook on/off by its full path (e.g., "Global/CGeneral/LimitAngle").
// Returns true if the toggle succeeded.
inline bool SetHookEnabled(const char* path, bool enabled) {
    auto r = ReversibleHooks::SetCategoryOrItemStateByPath(path, enabled);
    return r == ReversibleHooks::SetCatOrItemStateResult::Done;
}

// RAII guard: ensures hook is disabled for original code, restores previous state on destruction.
// Handles the case where --unhook-except already disabled the hook — the guard will
// enable it first (so reversed code is active), then disable (original), then restore.
struct HookDisableGuard {
    const char* path;
    bool needsRestore; // true if we need to re-enable on destruction
    bool valid;        // true if the hook path was found

    explicit HookDisableGuard(const char* hookPath) : path(hookPath), needsRestore(false), valid(false) {
        // First ensure the hook is enabled (reversed code active)
        auto enableResult = ReversibleHooks::SetCategoryOrItemStateByPath(path, true);
        if (enableResult == ReversibleHooks::SetCatOrItemStateResult::NotFound) {
            char msg[256];
            _snprintf(msg, sizeof(msg), "HookDisableGuard: hook '%s' not found", path);
            GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
            return;
        }
        if (enableResult == ReversibleHooks::SetCatOrItemStateResult::Locked) {
            char msg[256];
            _snprintf(msg, sizeof(msg), "HookDisableGuard: hook '%s' is locked", path);
            GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
            return;
        }
        valid = true;

        // Now disable (switches to original code)
        ReversibleHooks::SetCategoryOrItemStateByPath(path, false);
        needsRestore = true;
    }

    ~HookDisableGuard() {
        if (needsRestore) {
            // Re-enable (back to reversed code)
            ReversibleHooks::SetCategoryOrItemStateByPath(path, true);
        }
    }

    HookDisableGuard(const HookDisableGuard&) = delete;
    HookDisableGuard& operator=(const HookDisableGuard&) = delete;
};

// Call a function with its hook disabled (original game code), then re-enable.
// Usage: float orig = CallOriginal("Global/CGeneral/LimitAngle", CGeneral::LimitAngle, 90.0f);
template<typename Fn, typename... Args>
auto CallOriginal(const char* hookPath, Fn fn, Args&&... args) -> decltype(fn(std::forward<Args>(args)...)) {
    HookDisableGuard guard(hookPath);
    return fn(std::forward<Args>(args)...);
}

// Compare original vs reversed for a function that returns a value.
// Asserts they match within epsilon.
#define EXPECT_MATCHES_ORIGINAL(hookPath, fn, eps, ...) do { \
    auto _orig = CallOriginal(hookPath, fn, __VA_ARGS__); \
    auto _rev  = fn(__VA_ARGS__); \
    EXPECT_NEAR(_orig, _rev, eps); \
} while(0)

// Differential test macro — same as GAME_TEST but documents intent
#define GAME_DIFF_TEST(Class, Name) GAME_TEST(Class, Diff_##Name)

// ---------------------------------------------------------------------------
// Dual-CRT rand() seeding
// ---------------------------------------------------------------------------
//
// The exe has its own statically-linked CRT with a separate rand() state at
// 0x821B1E.  The DLL's srand() only seeds the DLL's CRT.  For differential
// tests that compare original (exe) code against reversed (DLL) code, we must
// seed BOTH CRTs so the same random sequence is produced.
//
// Exe's srand is at 0x821B11:
//   call __getptd
//   mov  ecx, [esp+4]
//   mov  [eax+0x14], ecx
//   ret

inline void ExeSrand(unsigned int seed) {
    using SrandFn = void(__cdecl*)(unsigned int);
    static auto fn = reinterpret_cast<SrandFn>(0x821B11);
    fn(seed);
}

// Seed both exe and DLL CRT rand states.
inline void SeedBothRng(unsigned int seed) {
    srand(seed);
    ExeSrand(seed);
}

// ---------------------------------------------------------------------------
// Direct-call helpers for struct-returning functions (sret workaround)
// ---------------------------------------------------------------------------
//
// HookDisableGuard doesn't work for functions returning structs > 4 bytes
// (CVector, CVector2D, etc.) due to MSVC x86 sret calling convention issues.
// Instead, call the original function directly at its game address using
// inline asm with explicit sret pointer setup.
//
// These helpers encapsulate the asm patterns so test files don't need to
// hand-roll inline assembly.

// Call a __cdecl function at `addr` that returns CVector2D via sret.
// Pattern: push args right-to-left, push sret pointer, call, add esp.
inline CVector2D CallOriginal_CVector2D(uint32 addr, const CVector2D& arg1) {
    CVector2D result = {};
    const CVector2D* pIn = &arg1;
    CVector2D* pOut = &result;
    __asm {
        mov eax, pIn
        push eax
        mov eax, pOut
        push eax
        mov eax, addr
        call eax
        add esp, 8
    }
    return result;
}

inline CVector2D CallOriginal_CVector2D(uint32 addr, const CVector2D& arg1, int32 arg2, int32 arg3) {
    CVector2D result = {};
    const CVector2D* pIn = &arg1;
    CVector2D* pOut = &result;
    __asm {
        push arg3
        push arg2
        mov eax, pIn
        push eax
        mov eax, pOut
        push eax
        mov eax, addr
        call eax
        add esp, 16
    }
    return result;
}

// Call a __thiscall function at `addr` that returns CVector via sret.
// MSVC thiscall sret: ecx = this, first stack arg = sret pointer, then visible args.
inline CVector CallOriginal_CVector_Thiscall(uint32 addr, void* thisPtr, int32 arg1) {
    CVector result = {};
    CVector* pOut = &result;
    __asm {
        push arg1
        mov eax, pOut
        push eax
        mov ecx, thisPtr
        mov eax, addr
        call eax
    }
    return result;
}

// Call a __thiscall function with CVector* output param (not sret — regular thiscall).
// void __thiscall Fn(this, CVector* out, int32 arg1, bool arg2)
inline void CallOriginal_OutVec_Thiscall(uint32 addr, void* thisPtr, CVector* out, int32 arg1, bool arg2) {
    int32 bArg = arg2 ? 1 : 0;
    __asm {
        push bArg
        push arg1
        push out
        mov ecx, thisPtr
        mov eax, addr
        call eax
    }
}
