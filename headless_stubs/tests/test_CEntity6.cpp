// test_CEntity6.cpp — More CEntity differential tests.
// Hook paths: Entity/CEntity/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Entity.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

// --- LivesInThisNonOverlapSector ---

GAME_DIFF_TEST(CEntity6, LivesInThisNonOverlapSector_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Use sector coords near the player — world coords map to sectors
    // Sector = (worldX + 3000) / 50, (worldY + 3000) / 50
    auto pos = player->GetPosition();
    int32 secX = (int32)((pos.x + 3000.f) / 50.f);
    int32 secY = (int32)((pos.y + 3000.f) / 50.f);
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/LivesInThisNonOverlapSector");
      orig = player->LivesInThisNonOverlapSector(secX, secY); }
    rev = player->LivesInThisNonOverlapSector(secX, secY);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity6, LivesInThisNonOverlapSector_Player_FarSector) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    // Use a far-away sector that the player definitely doesn't live in
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/LivesInThisNonOverlapSector");
      orig = player->LivesInThisNonOverlapSector(0, 0); }
    rev = player->LivesInThisNonOverlapSector(0, 0);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity6, LivesInThisNonOverlapSector_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    CVector spawnPos(2460.f, -1650.f, 13.5f);
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, spawnPos);
    if (!npc) return;
    int32 secX = (int32)((spawnPos.x + 3000.f) / 50.f);
    int32 secY = (int32)((spawnPos.y + 3000.f) / 50.f);
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/LivesInThisNonOverlapSector");
      orig = npc->LivesInThisNonOverlapSector(secX, secY); }
    rev = npc->LivesInThisNonOverlapSector(secX, secY);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity6, LivesInThisNonOverlapSector_Vehicle) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    CVector spawnPos(2462.f, -1650.f, 13.5f);
    Scenario::TestVehicle veh(model, spawnPos);
    if (!veh) return;
    int32 secX = (int32)((spawnPos.x + 3000.f) / 50.f);
    int32 secY = (int32)((spawnPos.y + 3000.f) / 50.f);
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/LivesInThisNonOverlapSector");
      orig = veh->LivesInThisNonOverlapSector(secX, secY); }
    rev = veh->LivesInThisNonOverlapSector(secX, secY);
    EXPECT_EQ(orig, rev);
}

// --- DoesNotCollideWithFlyers ---

GAME_DIFF_TEST(CEntity6, DoesNotCollideWithFlyers_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/DoesNotCollideWithFlyers");
      orig = player->DoesNotCollideWithFlyers(); }
    rev = player->DoesNotCollideWithFlyers();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity6, DoesNotCollideWithFlyers_NPC) {
    int32 model = Scenario::FindLoadedPedModel();
    if (model < 0) return;
    Scenario::TestPed npc(PED_TYPE_CIVMALE, model, CVector(2464.f, -1650.f, 13.5f));
    if (!npc) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/DoesNotCollideWithFlyers");
      orig = npc->DoesNotCollideWithFlyers(); }
    rev = npc->DoesNotCollideWithFlyers();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity6, DoesNotCollideWithFlyers_Vehicle) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2466.f, -1650.f, 13.5f));
    if (!veh) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/DoesNotCollideWithFlyers");
      orig = veh->DoesNotCollideWithFlyers(); }
    rev = veh->DoesNotCollideWithFlyers();
    EXPECT_EQ(orig, rev);
}

// --- GetModellingMatrix ---

GAME_DIFF_TEST(CEntity6, GetModellingMatrix_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    RwMatrix* orig;
    RwMatrix* rev;
    { HookDisableGuard guard("Entity/CEntity/GetModellingMatrix");
      orig = player->GetModellingMatrix(); }
    rev = player->GetModellingMatrix();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity6, GetModellingMatrix_Vehicle) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2468.f, -1650.f, 13.5f));
    if (!veh) return;
    RwMatrix* orig;
    RwMatrix* rev;
    { HookDisableGuard guard("Entity/CEntity/GetModellingMatrix");
      orig = veh->GetModellingMatrix(); }
    rev = veh->GetModellingMatrix();
    EXPECT_EQ(orig, rev);
}
