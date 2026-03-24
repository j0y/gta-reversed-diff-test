// test_CEntity5.cpp — CEntity screen/occlusion queries.
// Hook paths: Entity/CEntity/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Entity.h"
#include "Entity/Ped/Ped.h"

GAME_DIFF_TEST(CEntity5, GetIsOnScreen_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsOnScreen");
      orig = player->GetIsOnScreen(); }
    rev = player->GetIsOnScreen();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity5, GetIsBoundingBoxOnScreen_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsBoundingBoxOnScreen");
      orig = player->GetIsBoundingBoxOnScreen(); }
    rev = player->GetIsBoundingBoxOnScreen();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity5, IsEntityOccluded_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/IsEntityOccluded");
      orig = player->IsEntityOccluded(); }
    rev = player->IsEntityOccluded();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CEntity5, GetIsTypePhysical_Player) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetIsTypePhysical");
      orig = player->GetIsTypePhysical(); }
    rev = player->GetIsTypePhysical();
    EXPECT_EQ(orig, rev);
}
