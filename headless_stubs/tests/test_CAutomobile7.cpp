// test_CAutomobile7.cpp — Differential tests for bugs from bugs.tsv.
//
// Bug targets:
//   CAutomobile::ClearHeliOrientation (0x6A2460) — sets 0.0f instead of -1.0f sentinel
//   CCamera::SetNewPlayerWeaponMode (0x50BFB0) — m_nMinZoom and m_nMaxZoom swapped

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Automobile.h"
#include "Camera.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_a7 = nullptr;
static bool s_triedA7 = false;

static CAutomobile* GetA7() {
    if (!s_triedA7) {
        s_triedA7 = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_a7 = new Scenario::TestVehicle(model, CVector(2515.f, -1642.f, 13.5f));
    }
    if (!s_a7) return nullptr;
    auto* veh = s_a7->AsVehicle();
    if (veh && veh->IsAutomobile()) return static_cast<CAutomobile*>(veh);
    return nullptr;
}

// --- ClearHeliOrientation ---
// Bug: reversed sets m_fForcedOrientation = 0.0f, original stores -1.0f (the "no orientation" sentinel).
// The constructor (line 351) initializes to -1.0f and the header comment (line 216) says -1.0f.
// Test: set m_fForcedOrientation to a non-zero value, call ClearHeliOrientation with hook
// enabled (reversed) vs disabled (original), compare the resulting field value.

GAME_DIFF_TEST(CAutomobile7, ClearHeliOrientation) {
    auto* car = GetA7();
    if (!car) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No automobile"); return; }

    // Set a known non-zero orientation so the clear has a visible effect
    car->m_fForcedOrientation = 1.5f;

    // Call original code
    float origVal;
    {
        HookDisableGuard guard("Vehicle/CAutomobile/ClearHeliOrientation");
        car->ClearHeliOrientation();
        origVal = car->m_fForcedOrientation;
    }

    // Restore and call reversed code
    car->m_fForcedOrientation = 1.5f;
    car->ClearHeliOrientation();
    float revVal = car->m_fForcedOrientation;

    // Original should write -1.0f, reversed writes 0.0f — this should FAIL
    EXPECT_NEAR(origVal, revVal, 1e-5f);
}

// --- SetNewPlayerWeaponMode ---
// Bug: original ASM at 0x50BFB0 stores the maxZoom parameter into m_nMinZoom field
// and minZoom parameter into m_nMaxZoom field (swapped). The reversed code assigns
// them "correctly" by name, so with different maxZoom != minZoom values, the fields
// will differ between original and reversed.
// Test: call with distinct maxZoom=5, minZoom=3, compare the m_nMinZoom and m_nMaxZoom fields.

GAME_DIFF_TEST(CAutomobile7, SetNewPlayerWeaponMode_ZoomSwap) {
    // Save original state
    auto savedMode = TheCamera.m_PlayerWeaponMode;

    const int16 testMaxZoom = 5;
    const int16 testMinZoom = 3;

    // Call original code
    uint16 origMinZoom, origMaxZoom;
    {
        HookDisableGuard guard("Global/CCamera/SetNewPlayerWeaponMode");
        TheCamera.SetNewPlayerWeaponMode(MODE_AIMWEAPON, testMaxZoom, testMinZoom);
        origMinZoom = TheCamera.m_PlayerWeaponMode.m_nMinZoom;
        origMaxZoom = TheCamera.m_PlayerWeaponMode.m_nMaxZoom;
    }

    // Call reversed code
    TheCamera.SetNewPlayerWeaponMode(MODE_AIMWEAPON, testMaxZoom, testMinZoom);
    uint16 revMinZoom = TheCamera.m_PlayerWeaponMode.m_nMinZoom;
    uint16 revMaxZoom = TheCamera.m_PlayerWeaponMode.m_nMaxZoom;

    // If the bug exists, original stores maxZoom(5) → m_nMinZoom, minZoom(3) → m_nMaxZoom
    // while reversed stores minZoom(3) → m_nMinZoom, maxZoom(5) → m_nMaxZoom
    // So origMinZoom=5 vs revMinZoom=3 — this should FAIL
    EXPECT_EQ((int)origMinZoom, (int)revMinZoom);
    EXPECT_EQ((int)origMaxZoom, (int)revMaxZoom);

    // Restore original state
    TheCamera.m_PlayerWeaponMode = savedMode;
}
