// test_CDamageManager_diag.cpp — Diagnostic test for bug #8 false positive.
//
// The original diff test fails (orig=-1, rev=16 for door 0) but the previous
// diagnostic test PASSES. The difference: original test starts from hooked=0
// (--unhook-except), while the diagnostic ran after other tests had re-enabled
// the hook (hooked=1). This version reproduces the exact conditions by
// explicitly setting hooked=0 before running the HookDisableGuard sequence.

#include "StdInc.h"
#include "TestFramework.h"
#include "DamageManager.h"
#include "reversiblehooks/ReversibleHooks.h"
#include "reversiblehooks/HookCategory.h"
#include "reversiblehooks/RootHookCategory.h"
#include "reversiblehooks/ReversibleHook/Simple.h"

static FILE* OpenDiagLog() {
    FILE* f = fopen("C:\\diag_bug8.log", "w");
    if (!f) f = fopen("diag_bug8.log", "w");
    return f;
}

static void LogHex(FILE* f, const char* label, uintptr_t addr, int nbytes) {
    auto* p = reinterpret_cast<const uint8*>(addr);
    fprintf(f, "  %s @ 0x%08X:", label, (unsigned)addr);
    for (int i = 0; i < nbytes; i++) fprintf(f, " %02x", p[i]);
    fprintf(f, "\n");
}

// Direct call to a game address via __cdecl function pointer
static int CallAtAddr(uintptr_t addr, int arg) {
    using Fn = int(__cdecl*)(int);
    return reinterpret_cast<Fn>(addr)(arg);
}

GAME_TEST(CDamageManager_Diag, GetCarNodeIndexFromDoor) {
    FILE* f = OpenDiagLog();
    if (!f) { GetTestContext().RecordFailure(__FILE__, __LINE__, "Cannot open diag log"); return; }

    const char* hookPath = "Global/CDamageManager/GetCarNodeIndexFromDoor";
    const uintptr_t GAME_ADDR = 0x6C26F0;

    fprintf(f, "=== CDamageManager::GetCarNodeIndexFromDoor diagnostic v2 ===\n\n");

    // ── Find the Simple hook object ──
    auto& root = ReversibleHooks::GetRootCategory();
    auto* globalCat = root.FindSubcategory("Global");
    ReversibleHooks::HookCategory* dmgCat = globalCat ? globalCat->FindSubcategory("CDamageManager") : nullptr;
    auto hookItem = dmgCat ? dmgCat->FindItem("GetCarNodeIndexFromDoor") : nullptr;
    auto* simpleHook = hookItem ? dynamic_cast<ReversibleHooks::ReversibleHook::Simple*>(hookItem.get()) : nullptr;

    fprintf(f, "Hook lookup: Global=%s CDamageManager=%s Item=%s Simple=%s\n",
            globalCat ? "Y" : "N", dmgCat ? "Y" : "N",
            hookItem ? "Y" : "N", simpleHook ? "Y" : "N");

    if (!simpleHook) {
        fprintf(f, "ERROR: could not find Simple hook object\n");
        fclose(f);
        GetTestContext().RecordFailure(__FILE__, __LINE__, "Hook object not found");
        return;
    }

    uintptr_t gtaAddr = (uintptr_t)simpleHook->GetHookGTAAddress();
    uintptr_t libAddr = (uintptr_t)simpleHook->GetHookOurAddress();

    fprintf(f, "  GTA addr:   0x%08X (expected 0x006C26F0)\n", (unsigned)gtaAddr);
    fprintf(f, "  LIB addr:   0x%08X\n", (unsigned)libAddr);
    fprintf(f, "  HookedBytes:%u  LibHookedBytes:%u\n",
            simpleHook->m_iHookedBytes, simpleHook->m_iLibHookedBytes);

    // ── C++ function pointer address ──
    using FnType = eCarNodes(*)(eDoors);
    FnType cppFn = &CDamageManager::GetCarNodeIndexFromDoor;
    uintptr_t cppAddr;
    memcpy(&cppAddr, &cppFn, sizeof(cppAddr));
    fprintf(f, "  &GetCarNodeIndexFromDoor (C++) = 0x%08X\n", (unsigned)cppAddr);
    fprintf(f, "  C++ fn == libAddr? %s\n", (cppAddr == libAddr) ? "YES" : "NO — DIFFERENT!");

    // Log saved byte arrays from hook object
    fprintf(f, "\n--- Saved byte arrays ---\n");
    fprintf(f, "  m_OriginalFunctionContent (GTA original):");
    for (unsigned i = 0; i < simpleHook->m_iHookedBytes; i++)
        fprintf(f, " %02x", simpleHook->m_OriginalFunctionContent[i]);
    fprintf(f, "\n");

    fprintf(f, "  m_HookContent (JMP to lib):");
    auto* hc = reinterpret_cast<const uint8*>(&simpleHook->m_HookContent);
    for (unsigned i = 0; i < simpleHook->m_iHookedBytes; i++)
        fprintf(f, " %02x", hc[i]);
    fprintf(f, "\n");

    fprintf(f, "  m_LibOriginalFunctionContent (lib original):");
    for (unsigned i = 0; i < simpleHook->m_iLibHookedBytes; i++)
        fprintf(f, " %02x", simpleHook->m_LibOriginalFunctionContent[i]);
    fprintf(f, "\n");

    fprintf(f, "  m_LibHookContent (JMP to GTA):");
    auto* lhc = reinterpret_cast<const uint8*>(&simpleHook->m_LibHookContent);
    for (unsigned i = 0; i < simpleHook->m_iLibHookedBytes; i++)
        fprintf(f, " %02x", lhc[i]);
    fprintf(f, "\n");

    // ═══════════════════════════════════════════════════════════════════
    // TEST A: From hooked=1 (should pass, like before)
    // ═══════════════════════════════════════════════════════════════════
    fprintf(f, "\n\n========== TEST A: Starting from hooked=1 ==========\n");
    ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, true);  // ensure hooked=1
    fprintf(f, "  Initial: hooked=%d\n", (int)simpleHook->Hooked());
    LogHex(f, "GTA", gtaAddr, 8);
    LogHex(f, "LIB", libAddr, 8);
    if (cppAddr != libAddr) LogHex(f, "C++", cppAddr, 8);

    for (int d = 0; d < 6; d++) {
        int orig, rev;
        {
            HookDisableGuard guard(hookPath);
            fprintf(f, "  [A d=%d] in-guard: hooked=%d\n", d, (int)simpleHook->Hooked());
            LogHex(f, "GTA", gtaAddr, 8);
            LogHex(f, "LIB", libAddr, 8);
            if (cppAddr != libAddr) LogHex(f, "C++", cppAddr, 8);
            orig = (int)CDamageManager::GetCarNodeIndexFromDoor((eDoors)d);
        }
        rev = (int)CDamageManager::GetCarNodeIndexFromDoor((eDoors)d);
        const char* status = (orig == rev) ? "OK" : "MISMATCH";
        fprintf(f, "  [A d=%d] orig=%d rev=%d  %s\n", d, orig, rev, status);
    }

    // ═══════════════════════════════════════════════════════════════════
    // TEST B: From hooked=0 (reproduces original test conditions)
    // ═══════════════════════════════════════════════════════════════════
    fprintf(f, "\n\n========== TEST B: Starting from hooked=0 (--unhook-except) ==========\n");
    ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, false);  // force hooked=0
    fprintf(f, "  Initial: hooked=%d\n", (int)simpleHook->Hooked());
    LogHex(f, "GTA", gtaAddr, 8);
    LogHex(f, "LIB", libAddr, 8);
    if (cppAddr != libAddr) LogHex(f, "C++", cppAddr, 8);

    bool anyFail = false;
    for (int d = 0; d < 6; d++) {
        int orig, rev;

        fprintf(f, "\n  [B d=%d] --- pre-guard ---\n", d);
        fprintf(f, "    hooked=%d\n", (int)simpleHook->Hooked());
        LogHex(f, "  GTA", gtaAddr, 8);
        LogHex(f, "  LIB", libAddr, 8);
        if (cppAddr != libAddr) LogHex(f, "  C++", cppAddr, 8);

        {
            // Manually reproduce what HookDisableGuard does, logging each step
            fprintf(f, "    step1: enable(true)...\n");
            auto r1 = ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, true);
            fprintf(f, "    step1 result: %d  hooked=%d\n", (int)r1, (int)simpleHook->Hooked());
            LogHex(f, "  GTA", gtaAddr, 8);
            LogHex(f, "  LIB", libAddr, 8);
            if (cppAddr != libAddr) LogHex(f, "  C++", cppAddr, 8);

            fprintf(f, "    step2: disable(false)...\n");
            auto r2 = ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, false);
            fprintf(f, "    step2 result: %d  hooked=%d\n", (int)r2, (int)simpleHook->Hooked());
            LogHex(f, "  GTA", gtaAddr, 8);
            LogHex(f, "  LIB", libAddr, 8);
            if (cppAddr != libAddr) LogHex(f, "  C++", cppAddr, 8);

            // Now call original via C++ and via direct address
            orig = (int)CDamageManager::GetCarNodeIndexFromDoor((eDoors)d);
            int directOrig = CallAtAddr(GAME_ADDR, d);
            fprintf(f, "    C++ call(d=%d) = %d\n", d, orig);
            fprintf(f, "    direct 0x6C26F0(d=%d) = %d\n", d, directOrig);

            // Re-enable to restore state
            ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, true);
        }

        rev = (int)CDamageManager::GetCarNodeIndexFromDoor((eDoors)d);
        fprintf(f, "  [B d=%d] orig=%d rev=%d  %s\n", d, orig, rev,
                (orig == rev) ? "OK" : "MISMATCH");

        if (orig != rev) {
            anyFail = true;
            char msg[128];
            snprintf(msg, sizeof(msg), "TestB door=%d: orig=%d rev=%d", d, orig, rev);
            GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
        }

        // For test B, disable again for next iteration to stay in hooked=0 state
        ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, false);
    }

    // ═══════════════════════════════════════════════════════════════════
    // TEST C: From hooked=0, but call via direct address only (bypass lib thunk)
    // ═══════════════════════════════════════════════════════════════════
    fprintf(f, "\n\n========== TEST C: hooked=0, direct call at 0x6C26F0 only ==========\n");
    ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, false);

    for (int d = 0; d < 6; d++) {
        int origDirect;
        {
            // enable then disable (same as HookDisableGuard from hooked=0)
            ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, true);
            ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, false);

            origDirect = CallAtAddr(GAME_ADDR, d);

            ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, true);
        }
        int revDirect = CallAtAddr(GAME_ADDR, d);
        fprintf(f, "  [C d=%d] orig_direct=%d rev_direct=%d  %s\n",
                d, origDirect, revDirect, (origDirect == revDirect) ? "OK" : "MISMATCH");
        ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, false);
    }

    // Restore hook to enabled state
    ReversibleHooks::SetCategoryOrItemStateByPath(hookPath, true);

    fflush(f);
    fclose(f);
}
