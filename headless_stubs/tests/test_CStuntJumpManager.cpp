// test_CStuntJumpManager.cpp — CStuntJumpManager behavior tests.
// Hook paths: Global/CStuntJumpManager/<fn>
//
// All tested functions return void — behavior/state tests only.
// We verify Init/Shutdown/ShutdownForRestart affect state correctly.

#include "StdInc.h"
#include "TestFramework.h"
#include "StuntJumpManager.h"

// --- Init creates pool and sets active ---

GAME_TEST(CStuntJumpManager, Init_CreatesPool) {
    // Pool should already exist from game startup Init call
    EXPECT_TRUE(CStuntJumpManager::mp_poolStuntJumps != nullptr);
}

GAME_TEST(CStuntJumpManager, Init_Active) {
    // After Init, m_bActive should be true
    EXPECT_TRUE(CStuntJumpManager::m_bActive);
}

// --- ShutdownForRestart resets state ---

GAME_TEST(CStuntJumpManager, ShutdownForRestart_ResetsState) {
    // Save current state
    auto* savedPool = CStuntJumpManager::mp_poolStuntJumps;
    auto savedActive = CStuntJumpManager::mp_Active;
    auto savedHitReward = CStuntJumpManager::m_bHitReward;
    auto savedTimer = CStuntJumpManager::m_iTimer;
    auto savedJumpState = CStuntJumpManager::m_jumpState;
    auto savedNumJumps = CStuntJumpManager::m_iNumJumps;
    auto savedNumCompleted = CStuntJumpManager::m_iNumCompleted;

    if (!savedPool) return; // pool must exist

    CStuntJumpManager::ShutdownForRestart();

    // Verify reset state
    EXPECT_EQ((uintptr_t)CStuntJumpManager::mp_Active, (uintptr_t)nullptr);
    EXPECT_EQ(CStuntJumpManager::m_bHitReward, false);
    EXPECT_EQ(CStuntJumpManager::m_iTimer, 0u);
    EXPECT_EQ((int)CStuntJumpManager::m_jumpState, (int)eJumpState::START_POINT_INTERSECTED);
    EXPECT_EQ(CStuntJumpManager::m_iNumJumps, 0);
    EXPECT_EQ(CStuntJumpManager::m_iNumCompleted, 0u);

    // Restore state
    CStuntJumpManager::mp_Active = savedActive;
    CStuntJumpManager::m_bHitReward = savedHitReward;
    CStuntJumpManager::m_iTimer = savedTimer;
    CStuntJumpManager::m_jumpState = savedJumpState;
    CStuntJumpManager::m_iNumJumps = savedNumJumps;
    CStuntJumpManager::m_iNumCompleted = savedNumCompleted;
}

// --- Shutdown destroys pool ---

GAME_TEST(CStuntJumpManager, Shutdown_And_Init_Roundtrip) {
    // Verify pool exists
    EXPECT_TRUE(CStuntJumpManager::mp_poolStuntJumps != nullptr);

    // Shutdown should null the pool
    CStuntJumpManager::Shutdown();
    EXPECT_TRUE(CStuntJumpManager::mp_poolStuntJumps == nullptr);

    // Re-init to restore state for other tests
    CStuntJumpManager::Init();
    EXPECT_TRUE(CStuntJumpManager::mp_poolStuntJumps != nullptr);
}
