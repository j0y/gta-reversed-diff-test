// test_FxSystemBP.cpp — Differential tests for FxSystemBP_c.
// Hook paths: Fx/FxSystemBP_c/<fn>
//
// FX system blueprint — manages particle effect definitions.

#include "StdInc.h"
#include "TestFramework.h"
#include "Fx/FxSystemBP.h"
#include "Fx/FxManager.h"

// --- GetBoundingSphere ---

GAME_DIFF_TEST(FxSystemBP, GetBoundingSphere) {
    // Find a loaded FX system via FxManager
    auto& fxMgr = g_fxMan;
    // FxManager has FindFxSystemBP which returns a blueprint
    const char* fxNames[] = {
        "blood_heli", "gunflash", "exhale", "cigsmoke",
        "water_splash", "NONEXISTENT_FX"
    };
    for (const char* name : fxNames) {
        auto* bp = fxMgr.FindFxSystemBP(name);
        if (!bp) continue;

        auto* orig = (FxSphere_c*)nullptr;
        auto* rev = (FxSphere_c*)nullptr;
        { HookDisableGuard guard("Fx/FxSystemBP_c/GetBoundingSphere");
          orig = bp->GetBoundingSphere(); }
        rev = bp->GetBoundingSphere();

        // Both should return same pointer (or both null)
        EXPECT_EQ(orig, rev);
    }
}
