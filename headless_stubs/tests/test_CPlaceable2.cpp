// test_CPlaceable2.cpp — Extended CPlaceable tests.
// Hook paths: Entity/CPlaceable/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Placeable.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "ScenarioHelpers.h"

// --- SetPosn overloads ---

GAME_DIFF_TEST(CPlaceable2, SetPosn_XYZ_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector saved = player->GetPosition();

    { HookDisableGuard guard("Entity/CPlaceable/SetPosn-xyz");
      player->SetPosn(saved.x + 0.01f, saved.y, saved.z); }
    CVector origPos = player->GetPosition();

    player->SetPosn(saved.x, saved.y, saved.z); // restore
    player->SetPosn(saved.x + 0.01f, saved.y, saved.z);
    CVector revPos = player->GetPosition();

    EXPECT_NEAR(origPos.x, revPos.x, 1e-4f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-4f);
    EXPECT_NEAR(origPos.z, revPos.z, 1e-4f);

    player->SetPosn(saved.x, saved.y, saved.z);
}

GAME_DIFF_TEST(CPlaceable2, SetPosn_Vec_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector saved = player->GetPosition();
    CVector test = saved + CVector(0.01f, 0.0f, 0.0f);

    { HookDisableGuard guard("Entity/CPlaceable/SetPosn-vector");
      player->SetPosn(test); }
    CVector origPos = player->GetPosition();

    player->SetPosn(saved);
    player->SetPosn(test);
    CVector revPos = player->GetPosition();

    EXPECT_NEAR(origPos.x, revPos.x, 1e-4f);
    EXPECT_NEAR(origPos.y, revPos.y, 1e-4f);

    player->SetPosn(saved);
}

// --- SetHeading ---

GAME_DIFF_TEST(CPlaceable2, SetHeading_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    float saved = player->GetHeading();

    float testHeading = 1.5f;
    { HookDisableGuard guard("Entity/CPlaceable/SetHeading");
      player->SetHeading(testHeading); }
    float origHeading = player->GetHeading();

    player->SetHeading(saved);
    player->SetHeading(testHeading);
    float revHeading = player->GetHeading();

    EXPECT_NEAR(origHeading, revHeading, 1e-4f);
    player->SetHeading(saved);
}

// --- SetOrientation ---

GAME_DIFF_TEST(CPlaceable2, SetOrientation_XYZ_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CMatrix saved = player->GetMatrix();

    { HookDisableGuard guard("Entity/CPlaceable/SetOrientation-xyz");
      player->SetOrientation(0.0f, 0.0f, 1.0f); }
    CVector origFwd = player->GetForward();

    player->SetMatrix(saved);
    player->SetOrientation(0.0f, 0.0f, 1.0f);
    CVector revFwd = player->GetForward();

    EXPECT_NEAR(origFwd.x, revFwd.x, 1e-3f);
    EXPECT_NEAR(origFwd.y, revFwd.y, 1e-3f);
    EXPECT_NEAR(origFwd.z, revFwd.z, 1e-3f);

    player->SetMatrix(saved);
}
