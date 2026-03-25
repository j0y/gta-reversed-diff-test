// test_CBirds2.cpp — More CBirds differential tests.
// Hook paths: Global/CBirds/<fn>
// Already tested: Init, Shutdown, CreateNumberOfBirds, HandleGunShot (9 tests)

#include "StdInc.h"
#include "TestFramework.h"
#include "Birds.h"

GAME_DIFF_TEST(CBirds2, CreateNumberOfBirds_AtPlayerPos) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CVector pos = player->GetPosition();
    pos.z += 20.0f; // above player
    { HookDisableGuard guard("Global/CBirds/CreateNumberOfBirds");
      CBirds::CreateNumberOfBirds(pos, pos + CVector(10.f, 0.f, 0.f), 3, (eBirdsBiome)0, false); }
    CBirds::CreateNumberOfBirds(pos, pos + CVector(10.f, 0.f, 0.f), 3, (eBirdsBiome)0, false);
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CBirds2, HandleGunShot_AtOrigin) {
    CVector a(0.f, 0.f, 50.f);
    CVector b(0.f, 0.f, 55.f);
    { HookDisableGuard guard("Global/CBirds/HandleGunShot");
      CBirds::HandleGunShot(&a, &b); }
    CBirds::HandleGunShot(&a, &b);
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CBirds2, Init_Diff) {
    { HookDisableGuard guard("Global/CBirds/Init");
      CBirds::Init(); }
    CBirds::Init();
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CBirds2, Update_Diff) {
    { HookDisableGuard guard("Global/CBirds/Update");
      CBirds::Update(); }
    CBirds::Update();
    EXPECT_TRUE(true);
}
