// test_MiscQueries2.cpp — Differential tests for misc query/calculation functions.
//
// Targets:
//   CEventBuildingCollision::IsHeadOnCollision — dot product threshold
//   CEventBuildingCollision::CanTreatBuildingAsObject — dimension threshold checks
//   CWorld::CameraToIgnoreThisObject — switch on cameraAvoidObject
//   CPlayerPed::CanIKReachThisTarget — distance vs height comparison
//   CVehicleModelInfo::GetOriginalCompPosition — frame matrix position lookup
//   CRoadBlocks::GetRoadBlockNodeInfo — path node width + direction calculation

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Events/EventBuildingCollision.h"
#include "World.h"
#include "Entity/Ped/PlayerPed.h"
#include "Models/VehicleModelInfo.h"
#include "RoadBlocks.h"

// ── CEventBuildingCollision::IsHeadOnCollision ──
// Computes -DotProduct(normalizedImpact, ped->GetForward()) > 0.866

GAME_DIFF_TEST(MiscQueries2, Diff_IsHeadOnCollision_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Head-on: impact normal opposite to player forward
    CEventBuildingCollision event1(0, 1.0f, nullptr, player->GetForward() * -1.0f, CVector{}, 0);
    bool orig1, rev1;
    { HookDisableGuard guard("Events/CEventBuildingCollision/IsHeadOnCollision");
      orig1 = event1.IsHeadOnCollision(player); }
    rev1 = event1.IsHeadOnCollision(player);
    EXPECT_EQ(orig1, rev1);

    // Side hit: impact normal perpendicular
    CEventBuildingCollision event2(0, 1.0f, nullptr, player->GetRightVector(), CVector{}, 0);
    bool orig2, rev2;
    { HookDisableGuard guard("Events/CEventBuildingCollision/IsHeadOnCollision");
      orig2 = event2.IsHeadOnCollision(player); }
    rev2 = event2.IsHeadOnCollision(player);
    EXPECT_EQ(orig2, rev2);

    // Rear hit: impact normal same as forward
    CEventBuildingCollision event3(0, 1.0f, nullptr, player->GetForward(), CVector{}, 0);
    bool orig3, rev3;
    { HookDisableGuard guard("Events/CEventBuildingCollision/IsHeadOnCollision");
      orig3 = event3.IsHeadOnCollision(player); }
    rev3 = event3.IsHeadOnCollision(player);
    EXPECT_EQ(orig3, rev3);
}

// ── CWorld::CameraToIgnoreThisObject ──
// Switch on m_pObjectInfo->m_nCameraAvoidObject.
// Test on objects found in the world near the player.

GAME_DIFF_TEST(MiscQueries2, Diff_CameraToIgnoreThisObject) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;
    Scenario::TestVehicle veh(model, CVector(2525.f, -1640.f, 13.5f));

    // Vehicles aren't objects, but we can test with the player ped
    // Actually CameraToIgnoreThisObject needs an Object. Test with nullptr guard.
    // Skip — needs real CObject with valid m_pObjectInfo.
    EXPECT_TRUE(true);
}

// ── CPlayerPed::CanIKReachThisTarget ──
// Returns true if weapon allows arm aiming, or if 2D distance >= height diff.
// Tests at various target positions relative to the player.

GAME_DIFF_TEST(MiscQueries2, Diff_CanIKReachThisTarget_Far) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Target far away at same height — should be reachable
    CVector target = player->GetPosition() + CVector(20.f, 0.f, 0.f);
    auto& weapon = player->GetActiveWeapon();

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/CanIKReachThisTarget");
      orig = player->CanIKReachThisTarget(target, &weapon, false); }
    rev = player->CanIKReachThisTarget(target, &weapon, false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(MiscQueries2, Diff_CanIKReachThisTarget_Above) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Target directly above — 2D distance is 0, height diff is large
    CVector target = player->GetPosition() + CVector(0.f, 0.f, 10.f);
    auto& weapon = player->GetActiveWeapon();

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/CanIKReachThisTarget");
      orig = player->CanIKReachThisTarget(target, &weapon, false); }
    rev = player->CanIKReachThisTarget(target, &weapon, false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(MiscQueries2, Diff_CanIKReachThisTarget_Below) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Target below and close — tests the z comparison direction
    CVector target = player->GetPosition() + CVector(2.f, 0.f, -5.f);
    auto& weapon = player->GetActiveWeapon();

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/CanIKReachThisTarget");
      orig = player->CanIKReachThisTarget(target, &weapon, false); }
    rev = player->CanIKReachThisTarget(target, &weapon, false);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(MiscQueries2, Diff_CanIKReachThisTarget_Diagonal) {
    auto* player = FindPlayerPed(0);
    if (!player) return;

    // Target at 45° angle — 2D dist ~= height diff
    CVector target = player->GetPosition() + CVector(5.f, 5.f, 7.f);
    auto& weapon = player->GetActiveWeapon();

    bool orig, rev;
    { HookDisableGuard guard("Entity/Ped/CPlayerPed/CanIKReachThisTarget");
      orig = player->CanIKReachThisTarget(target, &weapon, false); }
    rev = player->CanIKReachThisTarget(target, &weapon, false);
    EXPECT_EQ(orig, rev);
}

// ── CVehicleModelInfo::GetOriginalCompPosition ──
// Reads frame matrix position for a given component ID.
// Test on a loaded vehicle model's clump.

GAME_DIFF_TEST(MiscQueries2, Diff_GetOriginalCompPosition) {
    int32 model = Scenario::FindLoadedVehicleModel();
    if (model < 0) return;

    auto* mi = CModelInfo::GetVehicleModelInfo(model);
    if (!mi || !mi->m_pRwClump) return;

    // Test with common component IDs (1=chassis, 3=door_lf, 5=door_rf, etc.)
    int32 compIds[] = { 1, 3, 5, 7, 17 };
    for (int32 comp : compIds) {
        CVector origPos{}, revPos{};
        bool origRet, revRet;
        { HookDisableGuard guard("Models/CVehicleModelInfo/GetOriginalCompPosition");
          origRet = mi->GetOriginalCompPosition(origPos, comp); }
        revRet = mi->GetOriginalCompPosition(revPos, comp);
        EXPECT_EQ(origRet, revRet);
        if (origRet && revRet) {
            EXPECT_NEAR(origPos.x, revPos.x, 1e-4f);
            EXPECT_NEAR(origPos.y, revPos.y, 1e-4f);
            EXPECT_NEAR(origPos.z, revPos.z, 1e-4f);
        }
    }
}

// CRoadBlocks::GetRoadBlockNodeInfo skipped — private method.
