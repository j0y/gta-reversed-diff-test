// test_CTrailer.cpp — Differential tests for CTrailer.
// Hook paths: Entity/Vehicle/CTrailer/<fn>
//
// CTrailer is a vehicle subtype. Test tow hitch/bar positions on
// a spawned trailer if one is loaded, or skip.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Vehicle/Trailer.h"
#include "Streaming.h"
#include "ScenarioHelpers.h"

// Trailer model IDs: 435 (farm trailer), 450 (fuel tanker), 584 (petrol tanker),
// 591 (articulated trailer), 606 (luggage trailer), 607 (stair trailer), 608 (baggage)
static int32 FindLoadedTrailerModel() {
    int32 trailers[] = { 435, 450, 584, 591, 606, 607, 608 };
    for (int32 id : trailers) {
        if (CStreaming::IsModelLoaded(id)) return id;
    }
    return -1;
}

// --- GetTowHitchPos ---

GAME_DIFF_TEST(CTrailer, GetTowHitchPos) {
    int32 model = FindLoadedTrailerModel();
    if (model < 0) return; // no trailer loaded at state 9

    Scenario::TestVehicle veh(model, CVector(2500.f, -1640.f, 13.5f));
    if (!veh) return;

    CVector origPos, revPos;
    bool origOk, revOk;

    { HookDisableGuard guard("Entity/Vehicle/CTrailer/GetTowHitchPos");
      origOk = veh->GetTowHitchPos(origPos, true, nullptr); }
    revOk = veh->GetTowHitchPos(revPos, true, nullptr);

    EXPECT_EQ(origOk, revOk);
    if (origOk && revOk) {
        EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
        EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
        EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
    }
}

// --- GetTowBarPos ---

GAME_DIFF_TEST(CTrailer, GetTowBarPos) {
    int32 model = FindLoadedTrailerModel();
    if (model < 0) return;

    Scenario::TestVehicle veh(model, CVector(2500.f, -1640.f, 13.5f));
    if (!veh) return;

    CVector origPos, revPos;
    bool origOk, revOk;

    { HookDisableGuard guard("Entity/Vehicle/CTrailer/GetTowBarPos");
      origOk = veh->GetTowBarPos(origPos, true, nullptr); }
    revOk = veh->GetTowBarPos(revPos, true, nullptr);

    EXPECT_EQ(origOk, revOk);
    if (origOk && revOk) {
        EXPECT_NEAR(origPos.x, revPos.x, 1e-3f);
        EXPECT_NEAR(origPos.y, revPos.y, 1e-3f);
        EXPECT_NEAR(origPos.z, revPos.z, 1e-3f);
    }
}
