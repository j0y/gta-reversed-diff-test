// test_Scenario_VehicleDamage.cpp — Scenario: vehicle in various damage states.
//
// Spawns a vehicle, applies specific damage via CDamageManager setters,
// then differentially tests getters and CVehicle queries under damage.
// Tests set→get round-trips and health threshold behavior.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "DamageManager.h"
#include "Entity/Vehicle/Automobile.h"

static Scenario::TestVehicle* s_dmgVeh = nullptr;
static bool s_dmgSetup = false;

static CAutomobile* GetAuto() {
    if (!s_dmgSetup) {
        s_dmgSetup = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_dmgVeh = new Scenario::TestVehicle(model, CVector(2460.0f, -1665.0f, 13.5f));
    }
    return s_dmgVeh ? s_dmgVeh->ptr : nullptr;
}

#define DMG_GUARD() if (!GetAuto()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No automobile"); return; }

// --- Set/Get round-trip differential tests ---

GAME_DIFF_TEST(Scenario_VehicleDamage, SetGetEngineStatus) {
    DMG_GUARD()
    auto& dmg = GetAuto()->m_damageManager;
    uint32 savedEngine = dmg.GetEngineStatus();

    uint32 testValues[] = { 0, 100, 200, 250 };
    for (uint32 val : testValues) {
        // Reversed set + get
        dmg.SetEngineStatus(val);
        uint32 rev = dmg.GetEngineStatus();

        // Original set + get
        { HookDisableGuard guard("Global/CDamageManager/SetEngineStatus");
          dmg.SetEngineStatus(val); }
        uint32 orig;
        { HookDisableGuard guard("Global/CDamageManager/GetEngineStatus");
          orig = dmg.GetEngineStatus(); }
        EXPECT_EQ(rev, orig);
    }
    dmg.SetEngineStatus(savedEngine);
}

GAME_DIFF_TEST(Scenario_VehicleDamage, SetGetDoorStatus) {
    DMG_GUARD()
    auto& dmg = GetAuto()->m_damageManager;

    for (uint8 d = DOOR_BONNET; d < MAX_DOORS; d++) {
        eDoorStatus savedStatus = dmg.GetDoorStatus((eDoors)d);

        eDoorStatus states[] = { DAMSTATE_OK, DAMSTATE_OPENED, DAMSTATE_DAMAGED, DAMSTATE_OPENED_DAMAGED };
        for (auto state : states) {
            dmg.SetDoorStatus((eDoors)d, state);
            int rev = (int)dmg.GetDoorStatus((eDoors)d);

            { HookDisableGuard guard("Global/CDamageManager/SetDoorStatus-");
              dmg.SetDoorStatus((eDoors)d, state); }
            int orig;
            { HookDisableGuard guard("Global/CDamageManager/GetDoorStatus");
              orig = (int)dmg.GetDoorStatus((eDoors)d); }
            EXPECT_EQ(rev, orig);
        }
        dmg.SetDoorStatus((eDoors)d, savedStatus);
    }
}

GAME_DIFF_TEST(Scenario_VehicleDamage, SetGetWheelStatus) {
    DMG_GUARD()
    auto& dmg = GetAuto()->m_damageManager;

    for (uint8 w = 0; w < 4; w++) {
        auto saved = dmg.GetWheelStatus((eCarWheel)w);

        uint8 states[] = { WHEEL_STATUS_OK, WHEEL_STATUS_BURST, WHEEL_STATUS_MISSING };
        for (auto state : states) {
            dmg.SetWheelStatus((eCarWheel)w, (eCarWheelStatus)state);
            int rev = (int)dmg.GetWheelStatus((eCarWheel)w);

            { HookDisableGuard guard("Global/CDamageManager/SetWheelStatus");
              dmg.SetWheelStatus((eCarWheel)w, (eCarWheelStatus)state); }
            int orig;
            { HookDisableGuard guard("Global/CDamageManager/GetWheelStatus");
              orig = (int)dmg.GetWheelStatus((eCarWheel)w); }
            EXPECT_EQ(rev, orig);
        }
        dmg.SetWheelStatus((eCarWheel)w, saved);
    }
}

GAME_DIFF_TEST(Scenario_VehicleDamage, SetGetPanelStatus) {
    DMG_GUARD()
    auto& dmg = GetAuto()->m_damageManager;

    for (uint8 p = 0; p < 7; p++) {
        auto saved = dmg.GetPanelStatus((ePanels)p);

        for (uint8 state = 0; state <= 3; state++) {
            dmg.SetPanelStatus((ePanels)p, (ePanelDamageState)state);
            int rev = (int)dmg.GetPanelStatus((ePanels)p);

            { HookDisableGuard guard("Global/CDamageManager/SetPanelStatus");
              dmg.SetPanelStatus((ePanels)p, (ePanelDamageState)state); }
            int orig;
            { HookDisableGuard guard("Global/CDamageManager/GetPanelStatus");
              orig = (int)dmg.GetPanelStatus((ePanels)p); }
            EXPECT_EQ(rev, orig);
        }
        dmg.SetPanelStatus((ePanels)p, (ePanelDamageState)saved);
    }
}

GAME_DIFF_TEST(Scenario_VehicleDamage, SetGetLightStatus) {
    DMG_GUARD()
    auto& dmg = GetAuto()->m_damageManager;

    for (uint8 l = 0; l < 4; l++) {
        auto saved = dmg.GetLightStatus((eLights)l);

        for (uint8 state = 0; state <= 1; state++) {
            dmg.SetLightStatus((eLights)l, (eLightsState)state);
            int rev = (int)dmg.GetLightStatus((eLights)l);

            { HookDisableGuard guard("Global/CDamageManager/SetLightStatus");
              dmg.SetLightStatus((eLights)l, (eLightsState)state); }
            int orig;
            { HookDisableGuard guard("Global/CDamageManager/GetLightStatus");
              orig = (int)dmg.GetLightStatus((eLights)l); }
            EXPECT_EQ(rev, orig);
        }
        dmg.SetLightStatus((eLights)l, (eLightsState)saved);
    }
}

// --- Damage progression differential tests ---

GAME_DIFF_TEST(Scenario_VehicleDamage, ProgressWheelDamage) {
    DMG_GUARD()
    auto& dmg = GetAuto()->m_damageManager;

    for (uint8 w = 0; w < 4; w++) {
        auto saved = dmg.GetWheelStatus((eCarWheel)w);

        // Start from OK, progress twice
        dmg.SetWheelStatus((eCarWheel)w, WHEEL_STATUS_OK);
        dmg.ProgressWheelDamage((eCarWheel)w);
        int revAfter1 = (int)dmg.GetWheelStatus((eCarWheel)w);
        dmg.ProgressWheelDamage((eCarWheel)w);
        int revAfter2 = (int)dmg.GetWheelStatus((eCarWheel)w);

        // Reset and do original
        dmg.SetWheelStatus((eCarWheel)w, WHEEL_STATUS_OK);
        { HookDisableGuard guard("Global/CDamageManager/ProgressWheelDamage");
          dmg.ProgressWheelDamage((eCarWheel)w); }
        int origAfter1 = (int)dmg.GetWheelStatus((eCarWheel)w);
        { HookDisableGuard guard("Global/CDamageManager/ProgressWheelDamage");
          dmg.ProgressWheelDamage((eCarWheel)w); }
        int origAfter2 = (int)dmg.GetWheelStatus((eCarWheel)w);

        EXPECT_EQ(revAfter1, origAfter1);
        EXPECT_EQ(revAfter2, origAfter2);

        dmg.SetWheelStatus((eCarWheel)w, saved);
    }
}

GAME_DIFF_TEST(Scenario_VehicleDamage, ProgressPanelDamage) {
    DMG_GUARD()
    auto& dmg = GetAuto()->m_damageManager;

    for (uint8 p = 0; p < 7; p++) {
        auto saved = dmg.GetPanelStatus((ePanels)p);

        // Start from 0, progress 3 times
        dmg.SetPanelStatus((ePanels)p, (ePanelDamageState)0);
        int revStates[3], origStates[3];
        for (int i = 0; i < 3; i++) {
            dmg.ProgressPanelDamage((ePanels)p);
            revStates[i] = (int)dmg.GetPanelStatus((ePanels)p);
        }

        dmg.SetPanelStatus((ePanels)p, (ePanelDamageState)0);
        for (int i = 0; i < 3; i++) {
            { HookDisableGuard guard("Global/CDamageManager/ProgressPanelDamage");
              dmg.ProgressPanelDamage((ePanels)p); }
            origStates[i] = (int)dmg.GetPanelStatus((ePanels)p);
        }

        for (int i = 0; i < 3; i++) {
            EXPECT_EQ(revStates[i], origStates[i]);
        }

        dmg.SetPanelStatus((ePanels)p, (ePanelDamageState)saved);
    }
}

// --- Vehicle-level queries under damage ---

GAME_DIFF_TEST(Scenario_VehicleDamage, IsVehicleNormal_Damaged) {
    DMG_GUARD()
    auto* car = GetAuto();
    float savedHealth = car->m_fHealth;

    // Full health — should be normal
    car->m_fHealth = 1000.0f;
    bool revNormal = car->IsVehicleNormal();
    bool origNormal;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal");
      origNormal = car->IsVehicleNormal(); }
    EXPECT_EQ(revNormal, origNormal);

    // Low health — may not be normal
    car->m_fHealth = 250.0f;
    bool revLow = car->IsVehicleNormal();
    bool origLow;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal");
      origLow = car->IsVehicleNormal(); }
    EXPECT_EQ(revLow, origLow);

    // Zero health
    car->m_fHealth = 0.0f;
    bool revDead = car->IsVehicleNormal();
    bool origDead;
    { HookDisableGuard guard("Vehicle/CVehicle/IsVehicleNormal");
      origDead = car->IsVehicleNormal(); }
    EXPECT_EQ(revDead, origDead);

    car->m_fHealth = savedHealth;
}

GAME_DIFF_TEST(Scenario_VehicleDamage, CanBeDriven_Damaged) {
    DMG_GUARD()
    auto* car = GetAuto();
    float savedHealth = car->m_fHealth;

    float healthValues[] = { 1000.0f, 500.0f, 250.0f, 0.0f };
    for (float h : healthValues) {
        car->m_fHealth = h;
        bool rev = car->CanBeDriven();
        bool orig;
        { HookDisableGuard guard("Vehicle/CVehicle/CanBeDriven");
          orig = car->CanBeDriven(); }
        EXPECT_EQ(rev, orig);
    }
    car->m_fHealth = savedHealth;
}

// --- Behavior tests ---

GAME_TEST(Scenario_VehicleDamage, HealthRange) {
    DMG_GUARD()
    auto* car = GetAuto();
    // Freshly spawned vehicle should have full health
    EXPECT_TRUE(car->m_fHealth > 0.0f);
    EXPECT_TRUE(car->m_fHealth <= 1000.0f);
}

GAME_TEST(Scenario_VehicleDamage, SetEngine_Clamps) {
    DMG_GUARD()
    auto& dmg = GetAuto()->m_damageManager;
    uint32 saved = dmg.GetEngineStatus();

    // Engine status is stored as uint8 internally (0-250 range)
    dmg.SetEngineStatus(0);
    EXPECT_EQ(dmg.GetEngineStatus(), 0u);

    dmg.SetEngineStatus(250);
    EXPECT_EQ(dmg.GetEngineStatus(), 250u);

    dmg.SetEngineStatus(saved);
}

GAME_TEST(Scenario_VehicleDamage, AllWheelsOK_AfterSet) {
    DMG_GUARD()
    auto& dmg = GetAuto()->m_damageManager;

    // Save
    eCarWheelStatus saved[4];
    for (int w = 0; w < 4; w++)
        saved[w] = dmg.GetWheelStatus((eCarWheel)w);

    // Set all to OK
    for (int w = 0; w < 4; w++)
        dmg.SetWheelStatus((eCarWheel)w, WHEEL_STATUS_OK);

    for (int w = 0; w < 4; w++)
        EXPECT_EQ((int)dmg.GetWheelStatus((eCarWheel)w), (int)WHEEL_STATUS_OK);

    // Restore
    for (int w = 0; w < 4; w++)
        dmg.SetWheelStatus((eCarWheel)w, saved[w]);
}
