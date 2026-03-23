// test_CEntity2.cpp — Extended differential tests for CEntity.
// Hook paths: Entity/CEntity/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Entity.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- HasPreRenderEffects ---

GAME_DIFF_TEST(CEntity2, HasPreRenderEffects_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/HasPreRenderEffects"); orig = player->HasPreRenderEffects(); }
    rev = player->HasPreRenderEffects();
    EXPECT_EQ(orig, rev);
}

// --- GetIsTypePhysical ---

GAME_DIFF_TEST(CEntity2, GetIsTypePhysical_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsTypePhysical"); orig = player->GetIsTypePhysical(); }
    rev = player->GetIsTypePhysical();
    EXPECT_EQ(orig, rev);
}

// --- TreatAsPlayerForCollisions ---

GAME_DIFF_TEST(CEntity2, TreatAsPlayerForCollisions_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/TreatAsPlayerForCollisions"); orig = player->TreatAsPlayerForCollisions(); }
    rev = player->TreatAsPlayerForCollisions();
    EXPECT_EQ(orig, rev);
}

// --- On spawned NPC ---

static Scenario::TestPed* s_entPed = nullptr;
static bool s_triedEntPed = false;

static CPed* GetEntityTestPed() {
    if (!s_triedEntPed) {
        s_triedEntPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_entPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2408.0f, -1672.0f, 13.5f));
    }
    return s_entPed ? s_entPed->AsPed() : nullptr;
}

GAME_DIFF_TEST(CEntity2, HasPreRenderEffects_NPC) {
    auto* ped = GetEntityTestPed();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/HasPreRenderEffects"); orig = ped->HasPreRenderEffects(); }
    rev = ped->HasPreRenderEffects();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity2, GetIsTypePhysical_NPC) {
    auto* ped = GetEntityTestPed();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsTypePhysical"); orig = ped->GetIsTypePhysical(); }
    rev = ped->GetIsTypePhysical();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity2, TreatAsPlayerForCollisions_NPC) {
    auto* ped = GetEntityTestPed();
    if (!ped) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/TreatAsPlayerForCollisions"); orig = ped->TreatAsPlayerForCollisions(); }
    rev = ped->TreatAsPlayerForCollisions();
    EXPECT_EQ(orig, rev);
}

// --- On spawned vehicle ---

static Scenario::TestVehicle* s_entVeh = nullptr;
static bool s_triedEntVeh = false;

static CVehicle* GetEntityTestVehicle() {
    if (!s_triedEntVeh) {
        s_triedEntVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0)
            s_entVeh = new Scenario::TestVehicle(model, CVector(2406.0f, -1668.0f, 13.5f));
    }
    return s_entVeh ? s_entVeh->AsVehicle() : nullptr;
}

GAME_DIFF_TEST(CEntity2, HasPreRenderEffects_Vehicle) {
    auto* veh = GetEntityTestVehicle();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/HasPreRenderEffects"); orig = veh->HasPreRenderEffects(); }
    rev = veh->HasPreRenderEffects();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity2, GetIsTypePhysical_Vehicle) {
    auto* veh = GetEntityTestVehicle();
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsTypePhysical"); orig = veh->GetIsTypePhysical(); }
    rev = veh->GetIsTypePhysical();
    EXPECT_EQ(orig, rev);
}
