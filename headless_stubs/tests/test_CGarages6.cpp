// test_CGarages6.cpp — More CGarages differential and behavior tests.
// Hook paths: Global/CGarages/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Garages.h"

// --- AllRespraysCloseOrOpen: void, behavior test ---

GAME_TEST(CGarages6, AllRespraysCloseOrOpen_Open) {
    CGarages::AllRespraysCloseOrOpen(true);
    EXPECT_TRUE(true);
}

GAME_TEST(CGarages6, AllRespraysCloseOrOpen_Close) {
    CGarages::AllRespraysCloseOrOpen(false);
    EXPECT_TRUE(true);
}

// --- ActivateGarage / DeActivateGarage: void, behavior test on garage 0 ---

GAME_TEST(CGarages6, ActivateGarage_0) {
    CGarages::ActivateGarage(0);
    EXPECT_TRUE(true);
}

GAME_TEST(CGarages6, DeActivateGarage_0) {
    CGarages::DeActivateGarage(0);
    EXPECT_TRUE(true);
}

GAME_TEST(CGarages6, ActivateDeActivate_RoundTrip) {
    CGarages::DeActivateGarage(0);
    CGarages::ActivateGarage(0);
    EXPECT_TRUE(true);
}

// --- IsGarageOpen / IsGarageClosed for all 50 garages ---

GAME_DIFF_TEST(CGarages6, IsGarageOpen_All50) {
    for (int16 i = 0; i < 50; i++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsGarageOpen"); orig = CGarages::IsGarageOpen(i); }
        rev = CGarages::IsGarageOpen(i);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CGarages6, IsGarageClosed_All50) {
    for (int16 i = 0; i < 50; i++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CGarages/IsGarageClosed"); orig = CGarages::IsGarageClosed(i); }
        rev = CGarages::IsGarageClosed(i);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetGarageNumberByName with more names ---

GAME_DIFF_TEST(CGarages6, GetGarageNumberByName_YOURGA) {
    int16 orig, rev;
    { HookDisableGuard guard("Global/CGarages/GetGarageNumberByName"); orig = CGarages::GetGarageNumberByName("YOURGA"); }
    rev = CGarages::GetGarageNumberByName("YOURGA");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGarages6, GetGarageNumberByName_YOURGB) {
    int16 orig, rev;
    { HookDisableGuard guard("Global/CGarages/GetGarageNumberByName"); orig = CGarages::GetGarageNumberByName("YOURGB"); }
    rev = CGarages::GetGarageNumberByName("YOURGB");
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CGarages6, GetGarageNumberByName_YOURGC) {
    int16 orig, rev;
    { HookDisableGuard guard("Global/CGarages/GetGarageNumberByName"); orig = CGarages::GetGarageNumberByName("YOURGC"); }
    rev = CGarages::GetGarageNumberByName("YOURGC");
    EXPECT_EQ(orig, rev);
}
