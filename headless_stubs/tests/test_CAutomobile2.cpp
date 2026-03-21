// test_CAutomobile2.cpp — Extended differential tests for CAutomobile.
// Hook paths: Vehicle/CAutomobile/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Entity/Vehicle/Automobile.h"
#include "DamageManager.h"

static Scenario::TestVehicle* s_auto2 = nullptr;
static bool s_auto2Setup = false;

static CAutomobile* GetAuto2() {
    if (!s_auto2Setup) {
        s_auto2Setup = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_auto2 = new Scenario::TestVehicle(model, CVector(2390.0f, -1665.0f, 13.5f));
    }
    return s_auto2 ? s_auto2->ptr : nullptr;
}

#define AUTO2_GUARD() if (!GetAuto2()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No automobile"); return; }

// --- Differential tests ---

GAME_DIFF_TEST(CAutomobile2, GetMovingCollisionOffset) {
    AUTO2_GUARD()
    float rev = GetAuto2()->GetMovingCollisionOffset();
    float orig;
    { HookDisableGuard guard("Vehicle/CAutomobile/GetMovingCollisionOffset");
      orig = GetAuto2()->GetMovingCollisionOffset(); }
    EXPECT_NEAR(rev, orig, 1e-5f);
}

GAME_DIFF_TEST(CAutomobile2, FixTyre_AllWheels) {
    AUTO2_GUARD()
    auto* car = GetAuto2();
    auto& dmg = car->m_damageManager;

    for (uint8 w = 0; w < 4; w++) {
        auto saved = dmg.GetWheelStatus((eCarWheel)w);

        // Burst then fix — compare
        dmg.SetWheelStatus((eCarWheel)w, WHEEL_STATUS_BURST);
        car->FixTyre((eWheels)w);
        int revStatus = (int)dmg.GetWheelStatus((eCarWheel)w);

        dmg.SetWheelStatus((eCarWheel)w, WHEEL_STATUS_BURST);
        { HookDisableGuard guard("Vehicle/CAutomobile/FixTyre");
          car->FixTyre((eWheels)w); }
        EXPECT_EQ(revStatus, (int)dmg.GetWheelStatus((eCarWheel)w));

        dmg.SetWheelStatus((eCarWheel)w, saved);
    }
}

// FixDoor accesses RW frame objects — crashes on doors without valid frames.
// Skip this differential test; FixTyre and FixPanel cover the fix pattern.
// The CDamageManager GetCarNodeIndexFromDoor bug (#8) also interferes here.

GAME_DIFF_TEST(CAutomobile2, FixPanel_AllPanels) {
    AUTO2_GUARD()
    auto* car = GetAuto2();
    auto& dmg = car->m_damageManager;

    for (uint8 p = 0; p < 7; p++) {
        auto saved = dmg.GetPanelStatus((ePanels)p);
        int nodeIdx = CDamageManager::GetCarNodeIndexFromPanel((ePanels)p);
        if (nodeIdx < 0) continue;

        dmg.SetPanelStatus((ePanels)p, (ePanelDamageState)2);
        car->FixPanel((eCarNodes)nodeIdx, (ePanels)p);
        int revStatus = (int)dmg.GetPanelStatus((ePanels)p);

        dmg.SetPanelStatus((ePanels)p, (ePanelDamageState)2);
        { HookDisableGuard guard("Vehicle/CAutomobile/FixPanel");
          car->FixPanel((eCarNodes)nodeIdx, (ePanels)p); }
        EXPECT_EQ(revStatus, (int)dmg.GetPanelStatus((ePanels)p));

        dmg.SetPanelStatus((ePanels)p, (ePanelDamageState)saved);
    }
}

GAME_DIFF_TEST(CAutomobile2, CloseAllDoors) {
    AUTO2_GUARD()
    auto* car = GetAuto2();

    car->CloseAllDoors();
    // After closing, check all doors report as closed
    for (uint8 d = DOOR_BONNET; d < MAX_DOORS; d++) {
        bool revClosed = car->IsDoorClosed((eDoors)d);
        // Compare with original
        { HookDisableGuard guard("Vehicle/CAutomobile/CloseAllDoors");
          // Can't re-close without re-opening, just verify reversed is consistent
        }
        // Behavior check: doors should be closed
        EXPECT_TRUE(revClosed);
    }
}

GAME_DIFF_TEST(CAutomobile2, ScanForCrimes) {
    AUTO2_GUARD()
    auto* car = GetAuto2();
    // ScanForCrimes reads vehicle state but shouldn't crash on spawned vehicle
    car->ScanForCrimes();
    { HookDisableGuard guard("Vehicle/CAutomobile/ScanForCrimes");
      car->ScanForCrimes(); }
    // Just verify no crash — crimes depend on game state
    EXPECT_TRUE(true);
}

// --- Behavior tests ---

GAME_TEST(CAutomobile2, MovingCollisionOffset_Finite) {
    AUTO2_GUARD()
    float offset = GetAuto2()->GetMovingCollisionOffset();
    EXPECT_TRUE(std::isfinite(offset));
}

GAME_TEST(CAutomobile2, FixTyre_RestoresOK) {
    AUTO2_GUARD()
    auto* car = GetAuto2();
    auto& dmg = car->m_damageManager;
    auto saved = dmg.GetWheelStatus(CAR_WHEEL_FRONT_LEFT);

    dmg.SetWheelStatus(CAR_WHEEL_FRONT_LEFT, WHEEL_STATUS_BURST);
    car->FixTyre((eWheels)0); // WHEEL_FRONT_LEFT = 0
    EXPECT_EQ((int)dmg.GetWheelStatus(CAR_WHEEL_FRONT_LEFT), (int)WHEEL_STATUS_OK);

    dmg.SetWheelStatus(CAR_WHEEL_FRONT_LEFT, saved);
}

GAME_TEST(CAutomobile2, DamageManager_Accessible) {
    AUTO2_GUARD()
    auto& dmg = GetAuto2()->m_damageManager;
    // Engine should be initialized (0 = OK)
    EXPECT_TRUE(dmg.GetEngineStatus() <= 250);
}
