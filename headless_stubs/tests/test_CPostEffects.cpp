// test_CPostEffects.cpp — Differential tests for CPostEffects.
// Hook paths: Global/CPostEffects/<fn>
//
// Most CPostEffects functions touch D3D9 render state. Only the
// script switches and state queries are safe to test headlessly.

#include "StdInc.h"
#include "TestFramework.h"
#include "PostEffects.h"

// --- IsVisionFXActive ---

GAME_DIFF_TEST(CPostEffects, IsVisionFXActive_Default) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CPostEffects/IsVisionFXActive");
      orig = CPostEffects::IsVisionFXActive(); }
    rev = CPostEffects::IsVisionFXActive();
    EXPECT_EQ(orig, rev);
}

// --- Script switches: toggle on then off, compare state ---

GAME_DIFF_TEST(CPostEffects, ScriptNightVisionSwitch) {
    // Save original state
    bool savedNV = CPostEffects::m_bNightVision;
    bool savedIR = CPostEffects::m_bInfraredVision;

    // Enable night vision via original
    { HookDisableGuard guard("Global/CPostEffects/ScriptNightVisionSwitch");
      CPostEffects::ScriptNightVisionSwitch(true); }
    bool origNV = CPostEffects::m_bNightVision;

    // Reset and enable via reversed
    CPostEffects::m_bNightVision = savedNV;
    CPostEffects::ScriptNightVisionSwitch(true);
    bool revNV = CPostEffects::m_bNightVision;

    EXPECT_EQ(origNV, revNV);

    // IsVisionFXActive should now return true
    bool origActive, revActive;
    { HookDisableGuard guard("Global/CPostEffects/IsVisionFXActive");
      origActive = CPostEffects::IsVisionFXActive(); }
    revActive = CPostEffects::IsVisionFXActive();
    EXPECT_EQ(origActive, revActive);

    // Disable
    { HookDisableGuard guard("Global/CPostEffects/ScriptNightVisionSwitch");
      CPostEffects::ScriptNightVisionSwitch(false); }
    bool origOff = CPostEffects::m_bNightVision;

    CPostEffects::m_bNightVision = true; // set to on first
    CPostEffects::ScriptNightVisionSwitch(false);
    bool revOff = CPostEffects::m_bNightVision;

    EXPECT_EQ(origOff, revOff);

    // Restore
    CPostEffects::m_bNightVision = savedNV;
    CPostEffects::m_bInfraredVision = savedIR;
}

GAME_DIFF_TEST(CPostEffects, ScriptInfraredVisionSwitch) {
    bool savedIR = CPostEffects::m_bInfraredVision;
    bool savedNV = CPostEffects::m_bNightVision;

    { HookDisableGuard guard("Global/CPostEffects/ScriptInfraredVisionSwitch");
      CPostEffects::ScriptInfraredVisionSwitch(true); }
    bool origIR = CPostEffects::m_bInfraredVision;

    CPostEffects::m_bInfraredVision = savedIR;
    CPostEffects::ScriptInfraredVisionSwitch(true);
    bool revIR = CPostEffects::m_bInfraredVision;

    EXPECT_EQ(origIR, revIR);

    // Disable
    { HookDisableGuard guard("Global/CPostEffects/ScriptInfraredVisionSwitch");
      CPostEffects::ScriptInfraredVisionSwitch(false); }
    bool origOff = CPostEffects::m_bInfraredVision;

    CPostEffects::m_bInfraredVision = true;
    CPostEffects::ScriptInfraredVisionSwitch(false);
    bool revOff = CPostEffects::m_bInfraredVision;

    EXPECT_EQ(origOff, revOff);

    CPostEffects::m_bInfraredVision = savedIR;
    CPostEffects::m_bNightVision = savedNV;
}

GAME_DIFF_TEST(CPostEffects, ScriptCCTVSwitch) {
    bool savedCCTV = CPostEffects::m_bCCTV;

    { HookDisableGuard guard("Global/CPostEffects/ScriptCCTVSwitch");
      CPostEffects::ScriptCCTVSwitch(true); }
    bool origOn = CPostEffects::m_bCCTV;

    CPostEffects::m_bCCTV = savedCCTV;
    CPostEffects::ScriptCCTVSwitch(true);
    bool revOn = CPostEffects::m_bCCTV;
    EXPECT_EQ(origOn, revOn);

    { HookDisableGuard guard("Global/CPostEffects/ScriptCCTVSwitch");
      CPostEffects::ScriptCCTVSwitch(false); }
    bool origOff = CPostEffects::m_bCCTV;

    CPostEffects::m_bCCTV = true;
    CPostEffects::ScriptCCTVSwitch(false);
    bool revOff = CPostEffects::m_bCCTV;
    EXPECT_EQ(origOff, revOff);

    CPostEffects::m_bCCTV = savedCCTV;
}

GAME_DIFF_TEST(CPostEffects, ScriptHeatHazeFXSwitch) {
    bool saved = CPostEffects::m_bHeatHazeFX;

    { HookDisableGuard guard("Global/CPostEffects/ScriptHeatHazeFXSwitch");
      CPostEffects::ScriptHeatHazeFXSwitch(true); }
    bool origOn = CPostEffects::m_bHeatHazeFX;

    CPostEffects::m_bHeatHazeFX = saved;
    CPostEffects::ScriptHeatHazeFXSwitch(true);
    bool revOn = CPostEffects::m_bHeatHazeFX;
    EXPECT_EQ(origOn, revOn);

    { HookDisableGuard guard("Global/CPostEffects/ScriptHeatHazeFXSwitch");
      CPostEffects::ScriptHeatHazeFXSwitch(false); }
    bool origOff = CPostEffects::m_bHeatHazeFX;

    CPostEffects::m_bHeatHazeFX = true;
    CPostEffects::ScriptHeatHazeFXSwitch(false);
    bool revOff = CPostEffects::m_bHeatHazeFX;
    EXPECT_EQ(origOff, revOff);

    CPostEffects::m_bHeatHazeFX = saved;
}

GAME_DIFF_TEST(CPostEffects, ScriptResetForEffects) {
    // Save state
    bool savedNV = CPostEffects::m_bNightVision;
    bool savedIR = CPostEffects::m_bInfraredVision;
    bool savedCCTV = CPostEffects::m_bCCTV;

    // Set some effects on
    CPostEffects::m_bNightVision = true;
    CPostEffects::m_bInfraredVision = true;

    // Reset via original
    { HookDisableGuard guard("Global/CPostEffects/ScriptResetForEffects");
      CPostEffects::ScriptResetForEffects(); }
    bool origNV = CPostEffects::m_bNightVision;
    bool origIR = CPostEffects::m_bInfraredVision;

    // Set again and reset via reversed
    CPostEffects::m_bNightVision = true;
    CPostEffects::m_bInfraredVision = true;
    CPostEffects::ScriptResetForEffects();
    bool revNV = CPostEffects::m_bNightVision;
    bool revIR = CPostEffects::m_bInfraredVision;

    EXPECT_EQ(origNV, revNV);
    EXPECT_EQ(origIR, revIR);

    // Restore
    CPostEffects::m_bNightVision = savedNV;
    CPostEffects::m_bInfraredVision = savedIR;
    CPostEffects::m_bCCTV = savedCCTV;
}

GAME_DIFF_TEST(CPostEffects, ScriptDarknessFilterSwitch) {
    bool saved = CPostEffects::m_bDarknessFilter;
    int32 savedAlpha = CPostEffects::m_DarknessFilterAlpha;

    int32 alphas[] = { 0, 50, 128, 200, 255 };
    for (int32 alpha : alphas) {
        CPostEffects::m_bDarknessFilter = false;
        CPostEffects::m_DarknessFilterAlpha = 0;
        { HookDisableGuard guard("Global/CPostEffects/ScriptDarknessFilterSwitch");
          CPostEffects::ScriptDarknessFilterSwitch(true, alpha); }
        bool origOn = CPostEffects::m_bDarknessFilter;
        int32 origAlpha = CPostEffects::m_DarknessFilterAlpha;

        CPostEffects::m_bDarknessFilter = false;
        CPostEffects::m_DarknessFilterAlpha = 0;
        CPostEffects::ScriptDarknessFilterSwitch(true, alpha);
        bool revOn = CPostEffects::m_bDarknessFilter;
        int32 revAlpha = CPostEffects::m_DarknessFilterAlpha;

        EXPECT_EQ(origOn, revOn);
        EXPECT_EQ(origAlpha, revAlpha);
    }

    CPostEffects::m_bDarknessFilter = saved;
    CPostEffects::m_DarknessFilterAlpha = savedAlpha;
}
