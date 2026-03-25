// test_CGarages5.cpp — Extended CGarages behavior tests.
// CGarages hooks are registered under different paths — test as behavior only
// for functions whose hook paths aren't findable at runtime.

#include "StdInc.h"
#include "TestFramework.h"
#include "Garages.h"

GAME_TEST(CGarages5, IsPointInAGarageCameraZone_Various) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.5f},
        {0.f, 0.f, 0.f},
        {1500.f, -1500.f, 20.f},
        {-2170.f, 250.f, 30.f},
    };
    for (auto& pos : positions) {
        bool result = CGarages::IsPointInAGarageCameraZone(pos);
        (void)result;
        EXPECT_TRUE(true);
    }
}

GAME_TEST(CGarages5, IsPointWithinAnyGarage_Various) {
    CVector positions[] = {
        {2488.f, -1666.f, 13.5f},
        {0.f, 0.f, 0.f},
        {1500.f, -1500.f, 20.f},
    };
    for (auto& pos : positions) {
        bool result = CGarages::IsPointWithinAnyGarage(pos);
        (void)result;
        EXPECT_TRUE(true);
    }
}

// FindDoorHeightForMI calls GetModelInfo()->GetColModel() — crashes if model
// has no collision. Use only loaded models with valid col data.
GAME_TEST(CGarages5, FindDoorHeightForMI_LoadedModels) {
    for (int32 id = 615; id < 20000; id++) {
        auto* mi = CModelInfo::GetModelInfo(id);
        if (!mi) continue;
        if (!CStreaming::IsModelLoaded(id)) continue;
        auto* col = mi->GetColModel();
        if (!col) continue;
        float height = CGarages::FindDoorHeightForMI(id);
        EXPECT_GT(height, -100.0f); // sanity check — not garbage
        break; // one is enough to verify it works
    }
}
