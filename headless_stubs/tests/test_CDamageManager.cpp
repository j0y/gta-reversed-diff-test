// test_CDamageManager.cpp — Differential tests for CDamageManager.
// Hook paths: Global/CDamageManager/<fn>
// Uses the damage manager from the spawned automobile.
//
// CDamageManager tracks vehicle damage state — doors, panels, lights, engine.
// Bugs here cause visual glitches and gameplay issues (invincible doors, etc).

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "DamageManager.h"
#include "Entity/Vehicle/Automobile.h"

static Scenario::TestVehicle* s_dmgVeh = nullptr;
static bool s_triedDmgVeh = false;

static CAutomobile* GetDmgAuto() {
    if (!s_triedDmgVeh) {
        s_triedDmgVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_dmgVeh = new Scenario::TestVehicle(model, CVector(2512.0f, -1665.0f, 13.5f));
        }
    }
    return s_dmgVeh ? s_dmgVeh->ptr : nullptr;
}

// --- Engine status ---

GAME_DIFF_TEST(CDamageManager, GetEngineStatus) {
    auto* car = GetDmgAuto();
    if (!car) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No automobile"); return; }
    auto& dmg = car->m_damageManager;
    uint32 orig, rev;
    { HookDisableGuard guard("Global/CDamageManager/GetEngineStatus"); orig = dmg.GetEngineStatus(); }
    rev = dmg.GetEngineStatus();
    EXPECT_EQ(orig, rev);
}

// --- Door status for all doors ---

GAME_DIFF_TEST(CDamageManager, GetDoorStatus_AllDoors) {
    auto* car = GetDmgAuto();
    if (!car) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No automobile"); return; }
    auto& dmg = car->m_damageManager;
    volatile auto fn = &CDamageManager::GetDoorStatus;
    for (uint8 d = DOOR_BONNET; d < MAX_DOORS; d++) {
        int orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetDoorStatus");
          orig = (int)(dmg.*fn)((eDoors)d); }
        rev = (int)(dmg.*fn)((eDoors)d);
        EXPECT_EQ(orig, rev);
    }
}

// --- Panel status ---

GAME_DIFF_TEST(CDamageManager, GetPanelStatus_AllPanels) {
    auto* car = GetDmgAuto();
    if (!car) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No automobile"); return; }
    auto& dmg = car->m_damageManager;
    // ePanels: 0-6
    for (uint8 p = 0; p < 7; p++) {
        int orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetPanelStatus");
          orig = (int)dmg.GetPanelStatus((ePanels)p); }
        rev = (int)dmg.GetPanelStatus((ePanels)p);
        EXPECT_EQ(orig, rev);
    }
}

// --- Light status ---

GAME_DIFF_TEST(CDamageManager, GetLightStatus_AllLights) {
    auto* car = GetDmgAuto();
    if (!car) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No automobile"); return; }
    auto& dmg = car->m_damageManager;
    // eLights: 0-3
    for (uint8 l = 0; l < 4; l++) {
        int orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetLightStatus");
          orig = (int)dmg.GetLightStatus((eLights)l); }
        rev = (int)dmg.GetLightStatus((eLights)l);
        EXPECT_EQ(orig, rev);
    }
}

// --- Static utility functions ---
// Use volatile function pointers to prevent MSVC from optimizing calls past
// the hookable thunk address in unity builds. Without this, the compiler can
// inline or devirtualize the call, bypassing the hook toggle entirely.

GAME_DIFF_TEST(CDamageManager, GetCarNodeIndexFromDoor) {
    volatile auto fn = &CDamageManager::GetCarNodeIndexFromDoor;
    // Expected: door 0→16, 1→17, 2→10, 3→8, 4→11, 5→9
    static constexpr int expected[] = { 16, 17, 10, 8, 11, 9 };
    for (uint8 d = DOOR_BONNET; d < MAX_DOORS; d++) {
        int orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetCarNodeIndexFromDoor");
          orig = (int)fn((eDoors)d); }
        rev = (int)fn((eDoors)d);
        EXPECT_EQ(orig, rev);
        EXPECT_EQ(rev, expected[d]);
    }
}

GAME_DIFF_TEST(CDamageManager, GetCarNodeIndexFromPanel) {
    volatile auto fn = &CDamageManager::GetCarNodeIndexFromPanel;
    // Expected: panel 0→15, 1→14, 2→-1, 3→-1, 4→18, 5→12, 6→13
    static constexpr int expected[] = { 15, 14, -1, -1, 18, 12, 13 };
    for (uint8 p = 0; p < 7; p++) {
        int orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetCarNodeIndexFromPanel");
          orig = (int)fn((ePanels)p); }
        rev = (int)fn((ePanels)p);
        EXPECT_EQ(orig, rev);
        EXPECT_EQ(rev, expected[p]);
    }
}

// --- Set/Get wheel status round-trip ---

GAME_DIFF_TEST(CDamageManager, SetGetWheelStatus) {
    auto* car = GetDmgAuto();
    if (!car) return;
    auto& dmg = car->m_damageManager;
    eCarWheel wheels[] = { CAR_WHEEL_FRONT_LEFT, CAR_WHEEL_REAR_LEFT, CAR_WHEEL_FRONT_RIGHT, CAR_WHEEL_REAR_RIGHT };
    for (auto w : wheels) {
        auto saved = dmg.GetWheelStatus(w);
        { HookDisableGuard guard("Global/CDamageManager/SetWheelStatus");
          dmg.SetWheelStatus(w, eCarWheelStatus::WHEEL_STATUS_OK); }
        auto origVal = (int32)dmg.GetWheelStatus(w);

        dmg.SetWheelStatus(w, saved);
        dmg.SetWheelStatus(w, eCarWheelStatus::WHEEL_STATUS_OK);
        auto revVal = (int32)dmg.GetWheelStatus(w);

        EXPECT_EQ(origVal, revVal);
        dmg.SetWheelStatus(w, saved);
    }
}

// --- GetAeroplaneCompStatus ---

GAME_DIFF_TEST(CDamageManager, GetAeroplaneCompStatus) {
    auto* car = GetDmgAuto();
    if (!car) return;
    auto& dmg = car->m_damageManager;
    for (int32 comp = 0; comp < 4; comp++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetAeroplaneCompStatus");
          orig = (int32)dmg.GetAeroplaneCompStatus(comp); }
        rev = (int32)dmg.GetAeroplaneCompStatus(comp);
        EXPECT_EQ(orig, rev);
    }
}

// --- SetEngineStatus ---

GAME_DIFF_TEST(CDamageManager, SetEngineStatus) {
    auto* car = GetDmgAuto();
    if (!car) return;
    auto& dmg = car->m_damageManager;
    auto saved = dmg.GetEngineStatus();

    { HookDisableGuard guard("Global/CDamageManager/SetEngineStatus");
      dmg.SetEngineStatus(200); }
    auto origVal = dmg.GetEngineStatus();

    dmg.SetEngineStatus(saved);
    dmg.SetEngineStatus(200);
    auto revVal = dmg.GetEngineStatus();

    EXPECT_EQ(origVal, revVal);
    dmg.SetEngineStatus(saved);
}

GAME_DIFF_TEST(CDamageManager, GetComponentGroup) {
    volatile auto fn = &CDamageManager::GetComponentGroup;
    for (uint8 comp = 0; comp < 20; comp++) {
        tComponentGroup groupOrig{}, groupRev{};
        uint8 idxOrig = 0, idxRev = 0;
        bool orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetComponentGroup");
          orig = fn((tComponent)comp, groupOrig, idxOrig); }
        rev = fn((tComponent)comp, groupRev, idxRev);
        if (orig != rev || (orig && ((int)groupOrig != (int)groupRev || idxOrig != idxRev))) {
            char msg[128];
            snprintf(msg, sizeof(msg), "comp=%d: orig=%d/%d/%d rev=%d/%d/%d",
                (int)comp, (int)orig, (int)groupOrig, (int)idxOrig, (int)rev, (int)groupRev, (int)idxRev);
            GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
            return;
        }
    }
}
