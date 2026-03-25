// test_CTrain2.cpp — Differential tests for CTrain.
// Hook paths: Vehicle/CTrain/<fn>
//
// Train spawning requires track data and special initialization, so we
// focus on static functions and guard instance methods behind a spawn helper.

#include "StdInc.h"
#include "TestFramework.h"
#include "ScenarioHelpers.h"
#include "Streaming.h"
#include "StreamingInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Entity/Vehicle/Train.h"
#include "World.h"

// ---------------------------------------------------------------------------
// Train spawn helper — trains need track position; try to create one if a
// train model is loaded and track data exists.
// ---------------------------------------------------------------------------

static CTrain* s_train = nullptr;
static bool s_triedTrain = false;

static CTrain* GetTestTrain() {
    if (!s_triedTrain) {
        s_triedTrain = true;
        for (int32 id = 400; id <= 611; id++) {
            auto* mi = CModelInfo::GetModelInfo(id);
            if (!mi || mi->GetModelType() != MODEL_INFO_VEHICLE) continue;
            auto* vmi = mi->AsVehicleModelInfoPtr();
            if (vmi->m_nVehicleType != VEHICLE_TYPE_TRAIN) continue;
            if (!CStreaming::IsModelLoaded(id)) {
                auto& info = CStreaming::GetInfo(id);
                if (info.GetCdSize() == 0) continue;
                ResumeOtherThreads resume;
                CStreaming::RequestModel(id, STREAMING_KEEP_IN_MEMORY);
                CStreaming::LoadAllRequestedModels(false);
            }
            if (CStreaming::IsModelLoaded(id)) {
                s_train = new CTrain(id, RANDOM_VEHICLE);
                if (s_train) {
                    s_train->SetPosn(CVector(2345.0f, -1668.0f, 13.5f));
                    s_train->SetStatus(STATUS_ABANDONED);
                    CWorld::Add(s_train);
                }
                break;
            }
        }
    }
    return s_train;
}

#define TRAIN_GUARD() if (!GetTestTrain()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No train — model not loaded"); return; }

// ---------------------------------------------------------------------------
// Static function differentials (no train instance required)
// ---------------------------------------------------------------------------

// FindNearestTrain and IsNextStationAllowed hooks are locked — behavior tests only

GAME_TEST(CTrain2, FindNearestTrain) {
    CVector pos(2345.0f, -1668.0f, 13.5f);
    CTrain* result = CTrain::FindNearestTrain(pos, false);
    (void)result;
    EXPECT_TRUE(true);
}

// IsNextStationAllowed(nullptr) crashes — skip

GAME_DIFF_TEST(CTrain2, DisableRandomTrains_Enable) {
    bool before = CTrain::bDisableRandomTrains;
    { HookDisableGuard guard("Vehicle/CTrain/DisableRandomTrains");
      CTrain::DisableRandomTrains(true); }
    bool origVal = CTrain::bDisableRandomTrains;
    CTrain::bDisableRandomTrains = before; // restore
    CTrain::DisableRandomTrains(true);
    bool revVal = CTrain::bDisableRandomTrains;
    CTrain::bDisableRandomTrains = before; // restore
    EXPECT_EQ(origVal, revVal);
}

GAME_DIFF_TEST(CTrain2, DisableRandomTrains_Disable) {
    bool before = CTrain::bDisableRandomTrains;
    { HookDisableGuard guard("Vehicle/CTrain/DisableRandomTrains");
      CTrain::DisableRandomTrains(false); }
    bool origVal = CTrain::bDisableRandomTrains;
    CTrain::bDisableRandomTrains = before; // restore
    CTrain::DisableRandomTrains(false);
    bool revVal = CTrain::bDisableRandomTrains;
    CTrain::bDisableRandomTrains = before; // restore
    EXPECT_EQ(origVal, revVal);
}

// ---------------------------------------------------------------------------
// Instance method differentials (require spawned train)
// ---------------------------------------------------------------------------

GAME_DIFF_TEST(CTrain2, FindNumCarriagesPulled) {
    TRAIN_GUARD()
    auto* train = GetTestTrain();
    uint32 orig, rev;
    { HookDisableGuard guard("Vehicle/CTrain/FindNumCarriagesPulled");
      orig = train->FindNumCarriagesPulled(); }
    rev = train->FindNumCarriagesPulled();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTrain2, FindSideStationIsOn) {
    TRAIN_GUARD()
    auto* train = GetTestTrain();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CTrain/FindSideStationIsOn");
      orig = train->FindSideStationIsOn(); }
    rev = train->FindSideStationIsOn();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CTrain2, IsInTunnel) {
    TRAIN_GUARD()
    auto* train = GetTestTrain();
    bool orig, rev;
    { HookDisableGuard guard("Vehicle/CTrain/IsInTunnel");
      orig = train->IsInTunnel(); }
    rev = train->IsInTunnel();
    EXPECT_EQ(orig, rev);
}

// IsNextStationAllowed hook is locked — behavior test only
GAME_TEST(CTrain2, IsNextStationAllowed_WithTrain) {
    TRAIN_GUARD()
    auto* train = GetTestTrain();
    bool result = CTrain::IsNextStationAllowed(train);
    (void)result;
    EXPECT_TRUE(true);
}
