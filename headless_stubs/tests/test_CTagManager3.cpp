// test_CTagManager3.cpp — More differential tests for CTagManager.
// Hook paths: Global/CTagManager/<fn>
// Note: FindTagDesc is private and cannot be tested from outside the class.

#include "StdInc.h"
#include "TestFramework.h"
#include "TagManager.h"

// --- IsTag ---

GAME_DIFF_TEST(CTagManager, IsTag_PlayerPed) {
    auto* ped = FindPlayerPed(0);
    bool orig, rev;
    { HookDisableGuard guard("Global/CTagManager/IsTag");
      orig = CTagManager::IsTag(ped); }
    rev = CTagManager::IsTag(ped);
    EXPECT_EQ(orig, rev);
}

// --- GetTagPos ---

GAME_DIFF_TEST(CTagManager, GetTagPos_Index0) {
    CVector orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetTagPos");
      orig = CTagManager::GetTagPos(0); }
    rev = CTagManager::GetTagPos(0);
    EXPECT_NEAR(orig.x, rev.x, 1e-5f);
    EXPECT_NEAR(orig.y, rev.y, 1e-5f);
    EXPECT_NEAR(orig.z, rev.z, 1e-5f);
}

GAME_DIFF_TEST(CTagManager, GetTagPos_Index1) {
    CVector orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetTagPos");
      orig = CTagManager::GetTagPos(1); }
    rev = CTagManager::GetTagPos(1);
    EXPECT_NEAR(orig.x, rev.x, 1e-5f);
    EXPECT_NEAR(orig.y, rev.y, 1e-5f);
    EXPECT_NEAR(orig.z, rev.z, 1e-5f);
}

GAME_DIFF_TEST(CTagManager, GetTagPos_Index2) {
    CVector orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetTagPos");
      orig = CTagManager::GetTagPos(2); }
    rev = CTagManager::GetTagPos(2);
    EXPECT_NEAR(orig.x, rev.x, 1e-5f);
    EXPECT_NEAR(orig.y, rev.y, 1e-5f);
    EXPECT_NEAR(orig.z, rev.z, 1e-5f);
}

GAME_DIFF_TEST(CTagManager, GetTagPos_Index3) {
    CVector orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetTagPos");
      orig = CTagManager::GetTagPos(3); }
    rev = CTagManager::GetTagPos(3);
    EXPECT_NEAR(orig.x, rev.x, 1e-5f);
    EXPECT_NEAR(orig.y, rev.y, 1e-5f);
    EXPECT_NEAR(orig.z, rev.z, 1e-5f);
}

GAME_DIFF_TEST(CTagManager, GetTagPos_Index4) {
    CVector orig, rev;
    { HookDisableGuard guard("Global/CTagManager/GetTagPos");
      orig = CTagManager::GetTagPos(4); }
    rev = CTagManager::GetTagPos(4);
    EXPECT_NEAR(orig.x, rev.x, 1e-5f);
    EXPECT_NEAR(orig.y, rev.y, 1e-5f);
    EXPECT_NEAR(orig.z, rev.z, 1e-5f);
}

// --- GetNearestTag ---

GAME_DIFF_TEST(CTagManager, GetNearestTag_GroveStreet) {
    CVector pos(2495.0f, -1667.0f, 13.0f);
    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Global/CTagManager/GetNearestTag");
      orig = CTagManager::GetNearestTag(pos); }
    rev = CTagManager::GetNearestTag(pos);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTagManager, GetNearestTag_NoTags) {
    CVector pos(0.0f, 0.0f, 0.0f);
    CEntity* orig;
    CEntity* rev;
    { HookDisableGuard guard("Global/CTagManager/GetNearestTag");
      orig = CTagManager::GetNearestTag(pos); }
    rev = CTagManager::GetNearestTag(pos);
    EXPECT_EQ(orig, rev);
}
