// test_CPedIK.cpp — Differential tests for CPedIK overloaded functions.
// Hook paths use dash: Global/CPedIK/MoveLimb-non-normalized, -normalized
//
// MoveLimb is a core IK function — tests inverse kinematics math.

#include "StdInc.h"
#include "TestFramework.h"
#include "PedIK.h"
#include "Entity/Ped/Ped.h"

// --- MoveLimb non-normalized ---

GAME_DIFF_TEST(CPedIK, MoveLimb_NonNormalized) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPedIK* ik = &player->m_pedIK;

    LimbOrientation origOrient{}, revOrient{};
    LimbMovementInfo moveInfo{};
    moveInfo.maxYaw = 1.0f;
    moveInfo.minYaw = -1.0f;
    moveInfo.maxPitch = 1.0f;
    moveInfo.minPitch = -1.0f;
    moveInfo.yawD = 0.1f;
    moveInfo.pitchD = 0.1f;

    float testYaw = 0.5f;
    float testPitch = 0.3f;

    origOrient = {};
    { HookDisableGuard guard("Global/CPedIK/MoveLimb-non-normalized");
      ik->MoveLimb(origOrient, testYaw, testPitch, moveInfo); }

    revOrient = {};
    int32 rev = (int32)ik->MoveLimb(revOrient, testYaw, testPitch, moveInfo);

    EXPECT_NEAR(origOrient.m_fYaw, revOrient.m_fYaw, 1e-5f);
    EXPECT_NEAR(origOrient.m_fPitch, revOrient.m_fPitch, 1e-5f);
}

// --- MoveLimb normalized (extra float param) ---

GAME_DIFF_TEST(CPedIK, MoveLimb_Normalized) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    CPedIK* ik = &player->m_pedIK;

    LimbOrientation origOrient{}, revOrient{};
    LimbMovementInfo moveInfo{};
    moveInfo.maxYaw = 1.0f;
    moveInfo.minYaw = -1.0f;
    moveInfo.maxPitch = 1.0f;
    moveInfo.minPitch = -1.0f;
    moveInfo.yawD = 0.1f;
    moveInfo.pitchD = 0.1f;

    float testYaw = 0.5f;
    float testPitch = 0.3f;
    float normFactor = 2.0f;

    origOrient = {};
    { HookDisableGuard guard("Global/CPedIK/MoveLimb-normalized");
      ik->MoveLimb(origOrient, testYaw, testPitch, moveInfo, normFactor); }

    revOrient = {};
    ik->MoveLimb(revOrient, testYaw, testPitch, moveInfo, normFactor);

    EXPECT_NEAR(origOrient.m_fYaw, revOrient.m_fYaw, 1e-5f);
    EXPECT_NEAR(origOrient.m_fPitch, revOrient.m_fPitch, 1e-5f);
}
