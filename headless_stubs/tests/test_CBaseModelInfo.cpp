// test_CBaseModelInfo.cpp — Differential tests for CBaseModelInfo queries.
// Hook paths: Models/CBaseModelInfo/<fn>
// Tests flag queries on model info entries accessed via CModelInfo::GetModelInfo().
//
// These queries determine rendering behavior — draw order, LOD, culling, etc.
// Bugs here cause visual glitches.

#include "StdInc.h"
#include "TestFramework.h"
#include "Models/BaseModelInfo.h"
#include "Models/ModelInfo.h"
#include "Streaming.h"

// Test a range of loaded model IDs
static void ForEachLoadedModel(int32 start, int32 end, int32 maxTests,
    void (*testFn)(CBaseModelInfo*, int32)) {
    int tested = 0;
    for (int32 id = start; id <= end && tested < maxTests; id++) {
        if (!CStreaming::IsModelLoaded(id)) continue;
        auto* mi = CModelInfo::GetModelInfo(id);
        if (!mi) continue;
        testFn(mi, id);
        tested++;
    }
    if (tested == 0) {
        GetTestContext().RecordFailure(__FILE__, __LINE__, "No loaded models found in range");
    }
}

// --- Flag queries ---

GAME_DIFF_TEST(CBaseModelInfo, GetIsDrawLast_Vehicles) {
    ForEachLoadedModel(400, 611, 20, [](CBaseModelInfo* mi, int32 id) {
        bool orig, rev;
        { HookDisableGuard guard("Models/CBaseModelInfo/GetIsDrawLast"); orig = mi->GetIsDrawLast(); }
        rev = mi->GetIsDrawLast();
        EXPECT_EQ(orig, rev);
    });
}

GAME_DIFF_TEST(CBaseModelInfo, IsBackfaceCulled_Vehicles) {
    ForEachLoadedModel(400, 611, 20, [](CBaseModelInfo* mi, int32 id) {
        bool orig, rev;
        { HookDisableGuard guard("Models/CBaseModelInfo/IsBackfaceCulled"); orig = mi->IsBackfaceCulled(); }
        rev = mi->IsBackfaceCulled();
        EXPECT_EQ(orig, rev);
    });
}

GAME_DIFF_TEST(CBaseModelInfo, IsLod_Vehicles) {
    ForEachLoadedModel(400, 611, 20, [](CBaseModelInfo* mi, int32 id) {
        bool orig, rev;
        { HookDisableGuard guard("Models/CBaseModelInfo/IsLod"); orig = mi->IsLod(); }
        rev = mi->IsLod();
        EXPECT_EQ(orig, rev);
    });
}

GAME_DIFF_TEST(CBaseModelInfo, IsRoad_Objects) {
    // Test object models (typically 615-20000 range, but also some low IDs)
    ForEachLoadedModel(615, 1000, 20, [](CBaseModelInfo* mi, int32 id) {
        bool orig, rev;
        { HookDisableGuard guard("Models/CBaseModelInfo/IsRoad"); orig = mi->IsRoad(); }
        rev = mi->IsRoad();
        EXPECT_EQ(orig, rev);
    });
}

GAME_DIFF_TEST(CBaseModelInfo, HasComplexHierarchy_Vehicles) {
    ForEachLoadedModel(400, 611, 20, [](CBaseModelInfo* mi, int32 id) {
        bool orig, rev;
        { HookDisableGuard guard("Models/CBaseModelInfo/HasComplexHierarchy"); orig = mi->HasComplexHierarchy(); }
        rev = mi->HasComplexHierarchy();
        EXPECT_EQ(orig, rev);
    });
}

GAME_DIFF_TEST(CBaseModelInfo, HasBeenPreRendered_Vehicles) {
    ForEachLoadedModel(400, 611, 20, [](CBaseModelInfo* mi, int32 id) {
        bool orig, rev;
        { HookDisableGuard guard("Models/CBaseModelInfo/HasBeenPreRendered"); orig = mi->HasBeenPreRendered(); }
        rev = mi->HasBeenPreRendered();
        EXPECT_EQ(orig, rev);
    });
}

GAME_DIFF_TEST(CBaseModelInfo, IsTagModel_Objects) {
    ForEachLoadedModel(615, 1000, 20, [](CBaseModelInfo* mi, int32 id) {
        bool orig, rev;
        { HookDisableGuard guard("Models/CBaseModelInfo/IsTagModel"); orig = mi->IsTagModel(); }
        rev = mi->IsTagModel();
        EXPECT_EQ(orig, rev);
    });
}

GAME_DIFF_TEST(CBaseModelInfo, IsBreakableStatuePart_Objects) {
    ForEachLoadedModel(615, 1000, 20, [](CBaseModelInfo* mi, int32 id) {
        bool orig, rev;
        { HookDisableGuard guard("Models/CBaseModelInfo/IsBreakableStatuePart"); orig = mi->IsBreakableStatuePart(); }
        rev = mi->IsBreakableStatuePart();
        EXPECT_EQ(orig, rev);
    });
}

// Get2dEffect crashes when models have no 2D effects (index 0 OOB) — skipped

// --- GetAnimFileIndex (VMT) ---

GAME_DIFF_TEST(CBaseModelInfo, GetAnimFileIndex_Vehicles) {
    ForEachLoadedModel(400, 611, 20, [](CBaseModelInfo* mi, int32 id) {
        int32 orig, rev;
        { HookDisableGuard guard("Models/CBaseModelInfo/GetAnimFileIndex"); orig = mi->GetAnimFileIndex(); }
        rev = mi->GetAnimFileIndex();
        EXPECT_EQ(orig, rev);
    });
}
