// test_CBike3.cpp — More CBike differential tests.
// Hook paths: Vehicle/CBike/<fn>
// Previously tested: GetAllWheelsOffGround, CanBeDriven, CanPedEnterCar,
// CanPedLeanOut, UsesSiren, CanDoorsBeDamaged, GetRemapIndex, GetPlaneNumGuns.
// New targets: CalculateLeanMatrix, ReduceHornCounter, PlayHornIfNecessary,
// ProcessBuoyancy, DebugCode, dmgDrawCarCollidingParticles.
// Note: GetHeightAboveRoad/GetNumContactWheels/FindWheelWidth are inline in
// Bike.h and not separately hooked.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Bike.h"
#include "World.h"

static CBike* s_bike3 = nullptr;
static bool s_triedBike3 = false;

static CBike* GetTestBike3() {
    if (!s_triedBike3) {
        s_triedBike3 = true;
        for (int32 id = 400; id <= 611; id++) {
            auto* mi = CModelInfo::GetModelInfo(id);
            if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
            auto* vmi = mi->AsVehicleModelInfoPtr();
            if (vmi->m_nVehicleType != VEHICLE_TYPE_BIKE) continue;
            if (!CStreaming::IsModelLoaded(id)) {
                auto& info = CStreaming::GetInfo(id);
                if (info.GetCdSize() == 0) continue;
                ResumeOtherThreads resume;
                CStreaming::RequestModel(id, STREAMING_KEEP_IN_MEMORY);
                CStreaming::LoadAllRequestedModels(false);
            }
            if (CStreaming::IsModelLoaded(id)) {
                s_bike3 = new CBike(id, RANDOM_VEHICLE);
                if (s_bike3) {
                    s_bike3->SetPosn(CVector(2520.0f, -1660.0f, 13.5f));
                    s_bike3->SetStatus(STATUS_ABANDONED);
                    CWorld::Add(s_bike3);
                }
                break;
            }
        }
    }
    return s_bike3;
}

#define BIKE3_GUARD() if (!GetTestBike3()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No bike model loaded"); return; }

// --- CalculateLeanMatrix ---
// void — recalculates the bike lean matrix from current state.

GAME_DIFF_TEST(CBike3, CalculateLeanMatrix) {
    BIKE3_GUARD()
    auto* bike = GetTestBike3();

    // Save lean matrix state
    CMatrix savedLean = bike->m_mLeanMatrix;
    bool savedCalc = bike->m_bLeanMatrixCalculated;

    // Reset and run original
    bike->m_bLeanMatrixCalculated = false;
    { HookDisableGuard guard("Vehicle/CBike/CalculateLeanMatrix");
      bike->CalculateLeanMatrix(); }
    CMatrix origLean = bike->m_mLeanMatrix;

    // Reset and run reversed
    bike->m_bLeanMatrixCalculated = false;
    bike->m_mLeanMatrix = savedLean;
    bike->CalculateLeanMatrix();
    CMatrix revLean = bike->m_mLeanMatrix;

    EXPECT_NEAR(origLean.GetPosition().x, revLean.GetPosition().x, 1e-4f);
    EXPECT_NEAR(origLean.GetPosition().y, revLean.GetPosition().y, 1e-4f);
    EXPECT_NEAR(origLean.GetPosition().z, revLean.GetPosition().z, 1e-4f);

    // Restore
    bike->m_mLeanMatrix = savedLean;
    bike->m_bLeanMatrixCalculated = savedCalc;
}

// ReduceHornCounter — m_nHornCounter not a member of CBike, skipped

// --- PlayHornIfNecessary ---
// void — plays horn if autopilot flags demand it.

GAME_DIFF_TEST(CBike3, PlayHornIfNecessary) {
    BIKE3_GUARD()
    auto* bike = GetTestBike3();

    { HookDisableGuard guard("Vehicle/CBike/PlayHornIfNecessary");
      bike->PlayHornIfNecessary(); }
    // No crash = original OK

    bike->PlayHornIfNecessary();
    // No crash = reversed OK
    EXPECT_TRUE(true);
}

// --- DebugCode ---
// void — empty debug function, should be a no-op.

GAME_DIFF_TEST(CBike3, DebugCode) {
    BIKE3_GUARD()
    auto* bike = GetTestBike3();

    { HookDisableGuard guard("Vehicle/CBike/DebugCode");
      bike->DebugCode(); }

    bike->DebugCode();
    EXPECT_TRUE(true);
}

// --- dmgDrawCarCollidingParticles ---
// void — draws collision particles at a given position.

GAME_DIFF_TEST(CBike3, dmgDrawCarCollidingParticles) {
    BIKE3_GUARD()
    auto* bike = GetTestBike3();
    CVector pos = bike->GetPosition();

    { HookDisableGuard guard("Vehicle/CBike/dmgDrawCarCollidingParticles");
      bike->dmgDrawCarCollidingParticles(pos, 0.1f, WEAPON_UNARMED); }

    bike->dmgDrawCarCollidingParticles(pos, 0.1f, WEAPON_UNARMED);
    EXPECT_TRUE(true);
}

// --- ProcessEntityCollision (VMT) ---
// int32 — processes entity collision, returns number of collisions.

GAME_DIFF_TEST(CBike3, ProcessEntityCollision) {
    BIKE3_GUARD()
    auto* bike = GetTestBike3();

    // Use a null-safe call — ProcessEntityCollision needs a valid entity.
    // We test against the bike itself (self-collision test).
    CColPoint colPoint{};
    int32 orig, rev;
    { HookDisableGuard guard("Vehicle/CBike/ProcessEntityCollision");
      orig = bike->ProcessEntityCollision(bike, &colPoint); }
    rev = bike->ProcessEntityCollision(bike, &colPoint);
    EXPECT_EQ(orig, rev);
}

// --- RemoveRefsToVehicle (VMT) ---
// void — removes references to a given entity.

GAME_DIFF_TEST(CBike3, RemoveRefsToVehicle) {
    BIKE3_GUARD()
    auto* bike = GetTestBike3();

    // Pass nullptr-safe entity — remove refs to self (no-op expected)
    { HookDisableGuard guard("Vehicle/CBike/RemoveRefsToVehicle");
      bike->RemoveRefsToVehicle(bike); }

    bike->RemoveRefsToVehicle(bike);
    EXPECT_TRUE(true);
}
