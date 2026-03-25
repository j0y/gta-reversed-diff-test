// test_CGlass4.cpp — More differential tests for CGlass.
// Hook paths: Global/CGlass/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Glass.h"

// --- CalcAlphaWithNormal (0x71ACF0) ---

GAME_DIFF_TEST(CGlass, CalcAlphaWithNormal_Various) {
    CVector normals[] = {
        CVector(0.0f, 0.0f, 1.0f),     // up
        CVector(0.0f, 1.0f, 0.0f),     // forward
        CVector(1.0f, 0.0f, 0.0f),     // right
        CVector(0.0f, 0.0f, -1.0f),    // down
        CVector(0.577f, 0.577f, 0.577f),   // normalized diagonal approx
        CVector(-0.577f, 0.577f, 0.577f),  // normalized diagonal approx
        CVector(0.707f, 0.707f, 0.0f),     // normalized XY diagonal
        CVector(0.0f, -0.707f, 0.707f),    // normalized YZ diagonal
    };
    for (auto& normal : normals) {
        uint8 orig, rev;
        { HookDisableGuard guard("Global/CGlass/CalcAlphaWithNormal");
          orig = CGlass::CalcAlphaWithNormal(normal); }
        rev = CGlass::CalcAlphaWithNormal(normal);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindFreePane (0x71ACA0) ---

GAME_DIFF_TEST(CGlass, FindFreePane_Basic) {
    CFallingGlassPane* orig;
    CFallingGlassPane* rev;
    { HookDisableGuard guard("Global/CGlass/FindFreePane");
      orig = CGlass::FindFreePane(); }
    rev = CGlass::FindFreePane();
    // Both should return the same pane (or both null)
    EXPECT_EQ(orig, rev);
}

// --- IsObjectGlass (0x46A760) ---

GAME_DIFF_TEST(CGlass, IsObjectGlass_PlayerPed) {
    // Player ped should not be glass
    CEntity* playerEntity = reinterpret_cast<CEntity*>(FindPlayerPed(0));
    if (playerEntity) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGlass/IsObjectGlass");
          orig = CGlass::IsObjectGlass(playerEntity); }
        rev = CGlass::IsObjectGlass(playerEntity);
        EXPECT_EQ(orig, rev);
    }
}

// IsObjectGlass(nullptr) crashes original code — skip
