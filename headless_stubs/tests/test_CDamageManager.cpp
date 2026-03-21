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
    for (uint8 d = DOOR_BONNET; d < MAX_DOORS; d++) {
        int orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetDoorStatus");
          orig = (int)dmg.GetDoorStatus((eDoors)d); }
        rev = (int)dmg.GetDoorStatus((eDoors)d);
        if (orig != rev) {
            char msg[128];
            snprintf(msg, sizeof(msg), "door=%d: orig=%d rev=%d", (int)d, orig, rev);
            GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
            return;
        }
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

GAME_DIFF_TEST(CDamageManager, GetCarNodeIndexFromDoor) {
    for (uint8 d = DOOR_BONNET; d < MAX_DOORS; d++) {
        int orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetCarNodeIndexFromDoor");
          orig = (int)CDamageManager::GetCarNodeIndexFromDoor((eDoors)d); }
        rev = (int)CDamageManager::GetCarNodeIndexFromDoor((eDoors)d);
        if (orig != rev) {
            char msg[128];
            snprintf(msg, sizeof(msg), "door=%d: orig=%d rev=%d", (int)d, orig, rev);
            GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
            return;
        }
    }
}

GAME_DIFF_TEST(CDamageManager, GetCarNodeIndexFromPanel) {
    for (uint8 p = 0; p < 7; p++) {
        int orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetCarNodeIndexFromPanel");
          orig = (int)CDamageManager::GetCarNodeIndexFromPanel((ePanels)p); }
        rev = (int)CDamageManager::GetCarNodeIndexFromPanel((ePanels)p);
        if (orig != rev) {
            char msg[128];
            snprintf(msg, sizeof(msg), "panel=%d: orig=%d rev=%d", (int)p, orig, rev);
            GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
            return;
        }
    }
}

GAME_DIFF_TEST(CDamageManager, GetComponentGroup) {
    for (uint8 comp = 0; comp < 20; comp++) {
        tComponentGroup groupOrig{}, groupRev{};
        uint8 idxOrig = 0, idxRev = 0;
        bool orig, rev;
        { HookDisableGuard guard("Global/CDamageManager/GetComponentGroup");
          orig = CDamageManager::GetComponentGroup((tComponent)comp, groupOrig, idxOrig); }
        rev = CDamageManager::GetComponentGroup((tComponent)comp, groupRev, idxRev);
        if (orig != rev || (orig && ((int)groupOrig != (int)groupRev || idxOrig != idxRev))) {
            char msg[128];
            snprintf(msg, sizeof(msg), "comp=%d: orig=%d/%d/%d rev=%d/%d/%d",
                (int)comp, (int)orig, (int)groupOrig, (int)idxOrig, (int)rev, (int)groupRev, (int)idxRev);
            GetTestContext().RecordFailure(__FILE__, __LINE__, msg);
            return;
        }
    }
}
