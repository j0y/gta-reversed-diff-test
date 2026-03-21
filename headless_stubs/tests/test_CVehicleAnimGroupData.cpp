// test_CVehicleAnimGroupData.cpp — Differential tests for CVehicleAnimGroupData.
// Hook paths: Global/CVehicleAnimGroupData/<fn>, Global/CVehicleAnimGroup/<fn>
//
// 30 vehicle animation groups loaded from handling.cfg at startup.
// All static functions index into the group array — pure lookups.

#include "StdInc.h"
#include "TestFramework.h"
#include "VehicleAnimGroupData.h"

static constexpr int32 NUM_GROUPS = CVehicleAnimGroupData::NUM_VEH_ANIM_GROUPS; // 30

// --- CVehicleAnimGroupData differential tests ---

GAME_DIFF_TEST(CVehicleAnimGroupData, GetInOutTimings) {
    // Both reversed and original use raw 0-29 indexing (88+ crashes original).
    // Compare directly to find struct layout / mode indexing bugs.
    eInOutTimingMode modes[] = { OPEN_START, OPEN_STOP, CLOST_START, CLOSE_STOP };
    for (int32 g = 0; g < NUM_GROUPS; g++) {
        for (auto mode : modes) {
            float revStart, revEnd, origStart, origEnd;
            CVehicleAnimGroupData::GetInOutTimings((AssocGroupId)g, mode, &revStart, &revEnd);
            { HookDisableGuard guard("Global/CVehicleAnimGroupData/GetInOutTimings");
              CVehicleAnimGroupData::GetInOutTimings((AssocGroupId)g, mode, &origStart, &origEnd); }
            EXPECT_NEAR(revStart, origStart, 1e-5f);
            EXPECT_NEAR(revEnd, origEnd, 1e-5f);
        }
    }
}

GAME_DIFF_TEST(CVehicleAnimGroupData, GetGroupForAnim) {
    // Test a representative set of anim IDs across all 30 groups
    AnimationId anims[] = {
        ANIM_ID_CAR_ALIGN_LHS, ANIM_ID_CAR_ALIGN_RHS,
        ANIM_ID_CAR_OPEN_LHS, ANIM_ID_CAR_OPEN_RHS,
        ANIM_ID_CAR_GETIN_LHS_0, ANIM_ID_CAR_GETIN_RHS_0,
        ANIM_ID_CAR_GETIN_LHS_1, ANIM_ID_CAR_GETIN_RHS_1,
        ANIM_ID_CAR_PULLOUT_LHS, ANIM_ID_CAR_PULLOUT_RHS,
        ANIM_ID_CAR_CLOSEDOOR_LHS_0, ANIM_ID_CAR_CLOSEDOOR_RHS_0,
        ANIM_ID_CAR_GETOUT_LHS_0, ANIM_ID_CAR_GETOUT_RHS_0,
        ANIM_ID_CAR_JACKEDLHS, ANIM_ID_CAR_JACKEDRHS,
        ANIM_ID_CAR_CLOSE_LHS_0, ANIM_ID_CAR_CLOSE_RHS_0,
        ANIM_ID_CAR_ROLLOUT_LHS, ANIM_ID_CAR_ROLLOUT_RHS,
        ANIM_ID_CAR_ROLLDOOR,
        ANIM_ID_CAR_FALLOUT_LHS, ANIM_ID_CAR_FALLOUT_RHS,
        ANIM_ID_CAR_DOORLOCKED_LHS, ANIM_ID_CAR_DOORLOCKED_RHS,
        ANIM_ID_CAR_SHUFFLE_RHS_0,
        ANIM_ID_CAR_GETIN_BIKE_FRONT,
    };
    for (int32 g = 0; g < NUM_GROUPS; g++) {
        for (auto anim : anims) {
            auto rev = CVehicleAnimGroupData::GetGroupForAnim((AssocGroupId)g, anim);
            AssocGroupId orig;
            { HookDisableGuard guard("Global/CVehicleAnimGroupData/GetGroupForAnim");
              orig = CVehicleAnimGroupData::GetGroupForAnim((AssocGroupId)g, anim); }
            EXPECT_EQ((int)rev, (int)orig);
        }
    }
}

GAME_DIFF_TEST(CVehicleAnimGroupData, GetAnimDoorOffset) {
    // The reversed GetAnimDoorOffset returns the raw cached offset (GetDoorOffset).
    // The original at 0x645600 may call ComputeAnimDoorOffsets (lazy compute from
    // animation data). If the offset hasn't been computed yet, reversed returns
    // zero while original returns the real value.
    eVehAnimDoorOffset doors[] = {
        ENTER_FRONT, ENTER_REAR, ENTER_BIKE_FRONT,
        EXIT_FRONT, EXIT_REAR, JACK_PED_LEFT, JACK_PED_RIGHT
    };
    for (int32 g = 0; g < NUM_GROUPS; g++) {
        for (auto door : doors) {
            CVector rev = CVehicleAnimGroupData::GetAnimDoorOffset((AssocGroupId)g, door);
            CVector orig;
            { HookDisableGuard guard("Global/CVehicleAnimGroupData/GetAnimDoorOffset");
              orig = CVehicleAnimGroupData::GetAnimDoorOffset((AssocGroupId)g, door); }
            EXPECT_NEAR(rev.x, orig.x, 1e-5f);
            EXPECT_NEAR(rev.y, orig.y, 1e-5f);
            EXPECT_NEAR(rev.z, orig.z, 1e-5f);
        }
    }
}

GAME_DIFF_TEST(CVehicleAnimGroupData, UsesTruckDrivingAnims) {
    for (int32 g = 0; g < NUM_GROUPS; g++) {
        bool rev = CVehicleAnimGroupData::UsesTruckDrivingAnims((AssocGroupId)g);
        bool orig;
        { HookDisableGuard guard("Global/CVehicleAnimGroupData/UsesTruckDrivingAnims");
          orig = CVehicleAnimGroupData::UsesTruckDrivingAnims((AssocGroupId)g); }
        EXPECT_EQ(rev, orig);
    }
}

GAME_DIFF_TEST(CVehicleAnimGroupData, UsesKartDrivingAnims) {
    for (int32 g = 0; g < NUM_GROUPS; g++) {
        bool rev = CVehicleAnimGroupData::UsesKartDrivingAnims((AssocGroupId)g);
        bool orig;
        { HookDisableGuard guard("Global/CVehicleAnimGroupData/UsesKartDrivingAnims");
          orig = CVehicleAnimGroupData::UsesKartDrivingAnims((AssocGroupId)g); }
        EXPECT_EQ(rev, orig);
    }
}

// --- CVehicleAnimGroup differential tests ---

GAME_DIFF_TEST(CVehicleAnimGroup, GetGroup) {
    AnimationId anims[] = {
        ANIM_ID_CAR_ALIGN_LHS, ANIM_ID_CAR_OPEN_LHS,
        ANIM_ID_CAR_GETIN_LHS_0, ANIM_ID_CAR_GETOUT_LHS_0,
        ANIM_ID_CAR_JACKEDLHS, ANIM_ID_CAR_ROLLOUT_LHS,
        ANIM_ID_CAR_FALLOUT_LHS, ANIM_ID_CAR_DOORLOCKED_LHS,
        ANIM_ID_CAR_ROLLDOOR, ANIM_ID_CAR_SHUFFLE_RHS_0,
    };
    for (int32 g = 0; g < NUM_GROUPS; g++) {
        auto& group = CVehicleAnimGroupData::GetVehicleAnimGroup(g);
        for (auto anim : anims) {
            auto rev = group.GetGroup(anim);
            AssocGroupId orig;
            { HookDisableGuard guard("Global/CVehicleAnimGroup/GetGroup");
              orig = group.GetGroup(anim); }
            EXPECT_EQ((int)rev, (int)orig);
        }
    }
}

GAME_DIFF_TEST(CVehicleAnimGroup, ComputeCriticalBlendTime) {
    AnimationId anims[] = {
        ANIM_ID_CAR_ALIGN_LHS, ANIM_ID_CAR_GETIN_LHS_0,
        ANIM_ID_CAR_GETOUT_LHS_0, ANIM_ID_CAR_JACKEDLHS,
        ANIM_ID_CAR_ROLLOUT_LHS, ANIM_ID_CAR_FALLOUT_LHS,
        ANIM_ID_CAR_CLOSE_LHS_0, // default case -> 0.0f
    };
    for (int32 g = 0; g < NUM_GROUPS; g++) {
        auto& group = CVehicleAnimGroupData::GetVehicleAnimGroup(g);
        for (auto anim : anims) {
            float rev = group.ComputeCriticalBlendTime(anim);
            float orig;
            { HookDisableGuard guard("Global/CVehicleAnimGroup/ComputeCriticalBlendTime");
              orig = group.ComputeCriticalBlendTime(anim); }
            EXPECT_NEAR(rev, orig, 1e-5f);
        }
    }
}

// --- Behavior tests ---

GAME_TEST(CVehicleAnimGroupData, GroupCount) {
    // All 30 groups should be initialized (firstGroup != 0 for most)
    int initialized = 0;
    for (int32 g = 0; g < NUM_GROUPS; g++) {
        auto& group = CVehicleAnimGroupData::GetVehicleAnimGroup(g);
        if (group.m_ucFirstGroup != 0 || group.m_ucSecondGroup != 0) {
            initialized++;
        }
    }
    // At least group 0 (STDCARAMIMS) should be initialized
    EXPECT_TRUE(initialized > 0);
}

GAME_TEST(CVehicleAnimGroupData, TimingsReasonable) {
    // Timings are loaded from handling.cfg — some may be negative by design
    // (e.g., indicating no animation). Just verify they're finite.
    for (int32 g = 0; g < NUM_GROUPS; g++) {
        auto& group = CVehicleAnimGroupData::GetVehicleAnimGroup(g);
        EXPECT_TRUE(std::isfinite(group.m_generalTiming.m_fGetInTime));
        EXPECT_TRUE(std::isfinite(group.m_generalTiming.m_fGetOutTime));
        EXPECT_TRUE(std::isfinite(group.m_generalTiming.m_fCarJackTime));
        EXPECT_TRUE(std::isfinite(group.m_generalTiming.m_fJumpOutTime));
        EXPECT_TRUE(std::isfinite(group.m_generalTiming.m_fFallOutTime));
    }
}
