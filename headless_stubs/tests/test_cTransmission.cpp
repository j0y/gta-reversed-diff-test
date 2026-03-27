// test_cTransmission.cpp — Differential tests for cTransmission.
// Hook paths: Global/cTransmission/<fn>
//
// Tests gear ratio initialization, simple gear shifting, and drive acceleration
// by comparing reversed vs original code on real handling data loaded from HANDLING.CFG.
// CalculateDriveAcceleration depends on CTimer::GetTimeStep() — both original and
// reversed see the same value so results should match exactly.

#include "StdInc.h"
#include "TestFramework.h"
#include "cTransmission.h"
#include "cHandlingDataMgr.h"
#include "Timer.h"

// Number of vehicle handling entries loaded from HANDLING.CFG
static constexpr int32 TRANS_NUM_VEHICLES = 210;

// Helper: get a valid transmission from handling ID, or nullptr if entry is empty
static cTransmission* GetTransmission(int32 id) {
    auto* h = gHandlingDataMgr.GetVehiclePointer(id);
    if (!h || h->m_fMass == 0.0f) return nullptr;
    return &h->GetTransmission();
}

// ---------------------------------------------------------------------------
// InitGearRatios — compare gear table produced by original vs reversed
// ---------------------------------------------------------------------------

GAME_DIFF_TEST(cTransmission, InitGearRatios_AllVehicles) {
    for (int32 id = 0; id < TRANS_NUM_VEHICLES; id++) {
        auto* src = GetTransmission(id);
        if (!src) continue;

        // Make two copies of the pre-init state
        cTransmission origCopy = *src;
        cTransmission revCopy = *src;

        // Original
        { HookDisableGuard guard("Global/cTransmission/InitGearRatios");
          origCopy.InitGearRatios(); }

        // Reversed
        revCopy.InitGearRatios();

        // Compare all 6 gear slots
        for (int g = 0; g <= origCopy.m_nNumberOfGears && g < 6; g++) {
            EXPECT_NEAR(origCopy.m_aGears[g].MaxVelocity,
                        revCopy.m_aGears[g].MaxVelocity, 1e-5f);
            EXPECT_NEAR(origCopy.m_aGears[g].ChangeUpVelocity,
                        revCopy.m_aGears[g].ChangeUpVelocity, 1e-5f);
            EXPECT_NEAR(origCopy.m_aGears[g].ChangeDownVelocity,
                        revCopy.m_aGears[g].ChangeDownVelocity, 1e-5f);
        }
    }
}

// ---------------------------------------------------------------------------
// CalculateGearForSimpleCar — gear selection at various speeds
// ---------------------------------------------------------------------------

GAME_DIFF_TEST(cTransmission, CalculateGearForSimpleCar_SweepSpeeds) {
    // Test across first 30 handling entries with a range of speeds and start gears
    for (int32 id = 0; id < 30; id++) {
        auto* trans = GetTransmission(id);
        if (!trans) continue;

        float speeds[] = { -0.05f, -0.01f, 0.0f, 0.001f, 0.05f, 0.1f, 0.2f,
                           0.4f, 0.6f, 0.8f, 1.0f, 1.5f, 2.0f };

        for (uint8 startGear = 0; startGear <= trans->m_nNumberOfGears && startGear < 6; startGear++) {
            for (float speed : speeds) {
                // Save and restore state since CalculateGearForSimpleCar modifies m_Velocity
                float savedVel = trans->m_Velocity;

                uint8 origGear = startGear;
                { HookDisableGuard guard("Global/cTransmission/CalculateGearForSimpleCar");
                  trans->CalculateGearForSimpleCar(speed, origGear); }
                float origVel = trans->m_Velocity;

                // Restore
                trans->m_Velocity = savedVel;

                uint8 revGear = startGear;
                trans->CalculateGearForSimpleCar(speed, revGear);
                float revVel = trans->m_Velocity;

                EXPECT_EQ(origGear, revGear);
                EXPECT_NEAR(origVel, revVel, 1e-6f);

                // Restore for next iteration
                trans->m_Velocity = savedVel;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CalculateDriveAcceleration — full acceleration calculation
// ---------------------------------------------------------------------------

GAME_DIFF_TEST(cTransmission, CalculateDriveAcceleration_BasicCases) {
    // Test with various gas pedal values, gears, and cheat modes
    float gasPedals[] = { 0.0f, 0.5f, 1.0f, -0.5f, -1.0f };
    uint8 cheatModes[] = { CHEAT_HANDLING_NONE, CHEAT_HANDLING_PERFECT, CHEAT_HANDLING_NITROS };

    for (int32 id = 0; id < 20; id++) {
        auto* trans = GetTransmission(id);
        if (!trans) continue;

        for (float gas : gasPedals) {
            for (uint8 cheat : cheatModes) {
                for (uint8 startGear = 1; startGear <= trans->m_nNumberOfGears && startGear < 6; startGear++) {
                    // Save transmission state
                    float savedVel = trans->m_Velocity;

                    // Set up identical inputs for both calls
                    float velocity = trans->m_aGears[startGear].MaxVelocity * 0.5f;
                    float origA6 = 0.0f, origA7 = 0.5f;
                    float revA6 = 0.0f, revA7 = 0.5f;
                    uint8 origGear = startGear, revGear = startGear;
                    float origGearChange = 0.0f, revGearChange = 0.0f;
                    float origVel = velocity, revVel = velocity;

                    float origResult, revResult;

                    { HookDisableGuard guard("Global/cTransmission/CalculateDriveAcceleration");
                      origResult = trans->CalculateDriveAcceleration(
                          gas, origGear, origGearChange, origVel,
                          &origA6, &origA7, 1, cheat); }

                    trans->m_Velocity = savedVel;

                    revResult = trans->CalculateDriveAcceleration(
                        gas, revGear, revGearChange, revVel,
                        &revA6, &revA7, 1, cheat);

                    EXPECT_NEAR(origResult, revResult, 1e-4f);
                    EXPECT_EQ(origGear, revGear);
                    EXPECT_NEAR(origA6, revA6, 1e-5f);
                    EXPECT_NEAR(origA7, revA7, 1e-5f);
                    EXPECT_NEAR(origVel, revVel, 1e-5f);

                    // Restore
                    trans->m_Velocity = savedVel;
                }
            }
        }
    }
}

GAME_DIFF_TEST(cTransmission, CalculateDriveAcceleration_Airborne) {
    // Test with allWheelsOnGround = 0 (different code path)
    for (int32 id = 0; id < 15; id++) {
        auto* trans = GetTransmission(id);
        if (!trans) continue;

        float savedVel = trans->m_Velocity;
        float gas = 1.0f;
        float velocity = trans->m_aGears[1].MaxVelocity * 0.3f;

        float origA6 = 0.0f, origA7 = 0.5f;
        float revA6 = 0.0f, revA7 = 0.5f;
        uint8 origGear = 1, revGear = 1;
        float origGearChange = 0.0f, revGearChange = 0.0f;
        float origVel = velocity, revVel = velocity;

        float origResult, revResult;

        { HookDisableGuard guard("Global/cTransmission/CalculateDriveAcceleration");
          origResult = trans->CalculateDriveAcceleration(
              gas, origGear, origGearChange, origVel,
              &origA6, &origA7, 0, CHEAT_HANDLING_NONE); }

        trans->m_Velocity = savedVel;

        revResult = trans->CalculateDriveAcceleration(
            gas, revGear, revGearChange, revVel,
            &revA6, &revA7, 0, CHEAT_HANDLING_NONE);

        EXPECT_NEAR(origResult, revResult, 1e-4f);
        EXPECT_EQ(origGear, revGear);
        EXPECT_NEAR(origA6, revA6, 1e-5f);
        EXPECT_NEAR(origA7, revA7, 1e-5f);

        trans->m_Velocity = savedVel;
    }
}

GAME_DIFF_TEST(cTransmission, CalculateDriveAcceleration_NullInertia) {
    // Test with a6/a7 = nullptr (skips inertia smoothing)
    for (int32 id = 0; id < 10; id++) {
        auto* trans = GetTransmission(id);
        if (!trans) continue;

        float savedVel = trans->m_Velocity;
        float gas = 1.0f;
        float velocity = trans->m_aGears[1].MaxVelocity * 0.5f;

        uint8 origGear = 2, revGear = 2;
        float origGearChange = 0.0f, revGearChange = 0.0f;
        float origVel = velocity, revVel = velocity;

        float origResult, revResult;

        { HookDisableGuard guard("Global/cTransmission/CalculateDriveAcceleration");
          origResult = trans->CalculateDriveAcceleration(
              gas, origGear, origGearChange, origVel,
              nullptr, nullptr, 1, CHEAT_HANDLING_NONE); }

        trans->m_Velocity = savedVel;

        revResult = trans->CalculateDriveAcceleration(
            gas, revGear, revGearChange, revVel,
            nullptr, nullptr, 1, CHEAT_HANDLING_NONE);

        EXPECT_NEAR(origResult, revResult, 1e-4f);
        EXPECT_EQ(origGear, revGear);

        trans->m_Velocity = savedVel;
    }
}

GAME_DIFF_TEST(cTransmission, CalculateDriveAcceleration_Reverse) {
    // Test reverse gear (currentGear = 0) with negative gas
    for (int32 id = 0; id < 15; id++) {
        auto* trans = GetTransmission(id);
        if (!trans) continue;

        float savedVel = trans->m_Velocity;
        float gas = -1.0f;
        float velocity = trans->m_aGears[0].MaxVelocity * 0.5f;

        float origA6 = 0.0f, origA7 = 0.5f;
        float revA6 = 0.0f, revA7 = 0.5f;
        uint8 origGear = 0, revGear = 0;
        float origGearChange = 0.0f, revGearChange = 0.0f;
        float origVel = velocity, revVel = velocity;

        float origResult, revResult;

        { HookDisableGuard guard("Global/cTransmission/CalculateDriveAcceleration");
          origResult = trans->CalculateDriveAcceleration(
              gas, origGear, origGearChange, origVel,
              &origA6, &origA7, 1, CHEAT_HANDLING_NONE); }

        trans->m_Velocity = savedVel;

        revResult = trans->CalculateDriveAcceleration(
            gas, revGear, revGearChange, revVel,
            &revA6, &revA7, 1, CHEAT_HANDLING_NONE);

        EXPECT_NEAR(origResult, revResult, 1e-4f);
        EXPECT_EQ(origGear, revGear);
        EXPECT_NEAR(origA6, revA6, 1e-4f);
        EXPECT_NEAR(origA7, revA7, 1e-4f);

        trans->m_Velocity = savedVel;
    }
}

// ---------------------------------------------------------------------------
// Behavior tests — sanity checks on gear ratio properties
// ---------------------------------------------------------------------------

GAME_TEST(cTransmission, GearRatiosMonotonicallyIncrease) {
    // Forward gears should have increasing MaxVelocity
    for (int32 id = 0; id < TRANS_NUM_VEHICLES; id++) {
        auto* trans = GetTransmission(id);
        if (!trans) continue;
        for (int g = 2; g <= trans->m_nNumberOfGears && g < 6; g++) {
            EXPECT_GT(trans->m_aGears[g].MaxVelocity,
                      trans->m_aGears[g - 1].MaxVelocity);
        }
    }
}

GAME_TEST(cTransmission, ReverseGearHasNegativeVelocity) {
    // Reverse gear (index 0) should have negative MaxVelocity
    for (int32 id = 0; id < TRANS_NUM_VEHICLES; id++) {
        auto* trans = GetTransmission(id);
        if (!trans) continue;
        EXPECT_LT(trans->m_aGears[0].MaxVelocity, 0.0f);
    }
}

GAME_TEST(cTransmission, GearCountInValidRange) {
    for (int32 id = 0; id < TRANS_NUM_VEHICLES; id++) {
        auto* trans = GetTransmission(id);
        if (!trans) continue;
        EXPECT_TRUE(trans->m_nNumberOfGears >= 1);
        EXPECT_TRUE(trans->m_nNumberOfGears <= 6);
    }
}

GAME_TEST(cTransmission, ChangeUpAboveChangeDown) {
    // For each forward gear, ChangeUpVelocity should be above ChangeDownVelocity
    for (int32 id = 0; id < TRANS_NUM_VEHICLES; id++) {
        auto* trans = GetTransmission(id);
        if (!trans) continue;
        for (int g = 1; g <= trans->m_nNumberOfGears && g < 6; g++) {
            EXPECT_GT(trans->m_aGears[g].ChangeUpVelocity,
                      trans->m_aGears[g].ChangeDownVelocity);
        }
    }
}
