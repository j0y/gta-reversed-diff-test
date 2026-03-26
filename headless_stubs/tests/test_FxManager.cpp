// test_FxManager.cpp — Differential tests for FxManager_c.
// Hook paths: Fx/FxManager_c/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Fx/FxManager.h"

// --- FindFxSystemBP ---

GAME_DIFF_TEST(FxManager, FindFxSystemBP_Blood) {
    FxSystemBP_c* orig;
    FxSystemBP_c* rev;
    { HookDisableGuard guard("Fx/FxManager_c/FindFxSystemBP");
      orig = g_fxMan.FindFxSystemBP("blood_heli"); }
    rev = g_fxMan.FindFxSystemBP("blood_heli");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(FxManager, FindFxSystemBP_Gunflash) {
    FxSystemBP_c* orig;
    FxSystemBP_c* rev;
    { HookDisableGuard guard("Fx/FxManager_c/FindFxSystemBP");
      orig = g_fxMan.FindFxSystemBP("gunflash"); }
    rev = g_fxMan.FindFxSystemBP("gunflash");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(FxManager, FindFxSystemBP_Unknown) {
    FxSystemBP_c* orig;
    FxSystemBP_c* rev;
    { HookDisableGuard guard("Fx/FxManager_c/FindFxSystemBP");
      orig = g_fxMan.FindFxSystemBP("nonexistent_fx_name"); }
    rev = g_fxMan.FindFxSystemBP("nonexistent_fx_name");
    EXPECT_EQ(orig, rev);
}

// --- FindFxSystemBP — sweep all common FX names ---

GAME_DIFF_TEST(FxManager, FindFxSystemBP_Sweep) {
    const char* names[] = {
        "exhale", "cigsmoke", "water_splash", "prt_smoke_huge",
        "prt_blood", "prt_spark", "prt_wake", "prt_gunshell",
        "fire_med", "fire_bike", "NONEXISTENT_1", "",
    };
    for (auto* name : names) {
        FxSystemBP_c* orig;
        FxSystemBP_c* rev;
        { HookDisableGuard guard("Fx/FxManager_c/FindFxSystemBP");
          orig = g_fxMan.FindFxSystemBP(name); }
        rev = g_fxMan.FindFxSystemBP(name);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetFrustumInfo ---

GAME_DIFF_TEST(FxManager, GetFrustumInfo) {
    FxFrustumInfo_c* orig;
    FxFrustumInfo_c* rev;
    { HookDisableGuard guard("Fx/FxManager_c/GetFrustumInfo");
      orig = g_fxMan.GetFrustumInfo(); }
    rev = g_fxMan.GetFrustumInfo();
    EXPECT_EQ(orig, rev);
}

// --- ShouldCreate — frustum culling for effects ---

GAME_DIFF_TEST(FxManager, ShouldCreate_NearCamera) {
    const char* fxNames[] = { "blood_heli", "gunflash", "exhale", "water_splash" };
    for (auto* name : fxNames) {
        auto* bp = g_fxMan.FindFxSystemBP(name);
        if (!bp) continue;

        RwMatrix transform;
        memset(&transform, 0, sizeof(transform));
        transform.right.x = 1.f;
        transform.up.y = 1.f;
        transform.at.z = 1.f;
        CVector pos = FindPlayerCoors(0);
        transform.pos = { pos.x, pos.y, pos.z };

        bool orig, rev;
        { HookDisableGuard guard("Fx/FxManager_c/ShouldCreate");
          orig = g_fxMan.ShouldCreate(bp, transform, nullptr, false); }
        rev = g_fxMan.ShouldCreate(bp, transform, nullptr, false);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(FxManager, ShouldCreate_FarAway) {
    auto* bp = g_fxMan.FindFxSystemBP("gunflash");
    if (!bp) return;

    RwMatrix transform;
    memset(&transform, 0, sizeof(transform));
    transform.right.x = 1.f;
    transform.up.y = 1.f;
    transform.at.z = 1.f;
    transform.pos = { 9999.f, 9999.f, 0.f };

    bool orig, rev;
    { HookDisableGuard guard("Fx/FxManager_c/ShouldCreate");
      orig = g_fxMan.ShouldCreate(bp, transform, nullptr, false); }
    rev = g_fxMan.ShouldCreate(bp, transform, nullptr, false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(FxManager, ShouldCreate_IgnoreBounding) {
    auto* bp = g_fxMan.FindFxSystemBP("gunflash");
    if (!bp) return;

    RwMatrix transform;
    memset(&transform, 0, sizeof(transform));
    transform.right.x = 1.f;
    transform.up.y = 1.f;
    transform.at.z = 1.f;
    transform.pos = { 9999.f, 9999.f, 0.f };

    bool orig, rev;
    { HookDisableGuard guard("Fx/FxManager_c/ShouldCreate");
      orig = g_fxMan.ShouldCreate(bp, transform, nullptr, true); }
    rev = g_fxMan.ShouldCreate(bp, transform, nullptr, true);
    EXPECT_EQ(orig, rev);
}

// --- FxRwMatrixCreate / FxRwMatrixDestroy ---

GAME_DIFF_TEST(FxManager, FxRwMatrixCreateDestroy) {
    RwMatrix* origMat;
    { HookDisableGuard guard("Fx/FxManager_c/FxRwMatrixCreate");
      origMat = g_fxMan.FxRwMatrixCreate(); }
    bool origOk = (origMat != nullptr);
    if (origMat) {
        { HookDisableGuard guard("Fx/FxManager_c/FxRwMatrixDestroy");
          g_fxMan.FxRwMatrixDestroy(origMat); }
    }

    RwMatrix* revMat = g_fxMan.FxRwMatrixCreate();
    bool revOk = (revMat != nullptr);
    if (revMat) g_fxMan.FxRwMatrixDestroy(revMat);

    EXPECT_EQ(origOk, revOk);
}

// --- SetWindData ---

GAME_DIFF_TEST(FxManager, SetWindData) {
    // m_pWindDir and m_pfWindSpeed are pointers to external data.
    // SetWindData stores the pointers, not the values.
    CVector* savedDir = g_fxMan.m_pWindDir;
    float* savedSpeed = g_fxMan.m_pfWindSpeed;

    CVector testDir(1.f, 0.f, 0.f);
    float testSpeed = 2.0f;

    // Original
    { HookDisableGuard guard("Fx/FxManager_c/SetWindData");
      g_fxMan.SetWindData(&testDir, &testSpeed); }
    CVector* origDir = g_fxMan.m_pWindDir;
    float* origSpeed = g_fxMan.m_pfWindSpeed;

    // Restore
    g_fxMan.m_pWindDir = savedDir;
    g_fxMan.m_pfWindSpeed = savedSpeed;

    // Reversed
    g_fxMan.SetWindData(&testDir, &testSpeed);
    CVector* revDir = g_fxMan.m_pWindDir;
    float* revSpeed = g_fxMan.m_pfWindSpeed;

    EXPECT_EQ(origDir, revDir);
    EXPECT_EQ(origSpeed, revSpeed);

    // Restore
    g_fxMan.m_pWindDir = savedDir;
    g_fxMan.m_pfWindSpeed = savedSpeed;
}

// --- CreateFxSystem / DestroyFxSystem — lifecycle across FX types ---

GAME_DIFF_TEST(FxManager, CreateDestroyFxSystem) {
    const char* names[] = { "blood_heli", "gunflash", "exhale", "water_splash" };
    for (auto* name : names) {
        if (!g_fxMan.FindFxSystemBP(name)) continue;

        CVector pos(0.f, 0.f, 10.f);

        FxSystem_c* origSys;
        { HookDisableGuard guard("Fx/FxManager_c/CreateFxSystem-1");
          origSys = g_fxMan.CreateFxSystem(name, pos, nullptr, true); }
        bool origOk = (origSys != nullptr);
        if (origSys) {
            { HookDisableGuard guard("Fx/FxManager_c/DestroyFxSystem");
              g_fxMan.DestroyFxSystem(origSys); }
        }

        FxSystem_c* revSys = g_fxMan.CreateFxSystem(name, pos, nullptr, true);
        bool revOk = (revSys != nullptr);
        if (revSys) g_fxMan.DestroyFxSystem(revSys);

        EXPECT_EQ(origOk, revOk);
    }
}

// --- Behavior tests ---

GAME_TEST(FxManager, BlueprintsLoaded) {
    const char* names[] = { "blood_heli", "gunflash", "exhale", "cigsmoke", "water_splash" };
    int found = 0;
    for (auto* name : names) {
        if (g_fxMan.FindFxSystemBP(name)) found++;
    }
    EXPECT_TRUE(found >= 3);
}

GAME_TEST(FxManager, MatrixPoolWorks) {
    RwMatrix* mats[5];
    for (int i = 0; i < 5; i++) {
        mats[i] = g_fxMan.FxRwMatrixCreate();
        EXPECT_TRUE(mats[i] != nullptr);
    }
    for (int i = 0; i < 5; i++) {
        if (mats[i]) g_fxMan.FxRwMatrixDestroy(mats[i]);
    }
}
