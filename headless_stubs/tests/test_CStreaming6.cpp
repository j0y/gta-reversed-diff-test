// test_CStreaming6.cpp — CStreaming mutation tests.
// Hook paths: Global/CStreaming/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"

// --- SetModelIsDeletable ---

GAME_DIFF_TEST(CStreaming6, SetModelIsDeletable_LoadedModel) {
    for (int32 id = 400; id < 500; id++) {
        auto& info = CStreaming::ms_aInfoForModel[id];
        if (!info.IsLoaded()) continue;

        auto saved = info.GetFlags();

        { HookDisableGuard guard("Global/CStreaming/SetModelIsDeletable");
          CStreaming::SetModelIsDeletable(id); }
        auto origFlags = info.GetFlags();

        info.SetFlags(saved);
        CStreaming::SetModelIsDeletable(id);
        auto revFlags = info.GetFlags();

        EXPECT_EQ(origFlags, revFlags);
        info.SetFlags(saved);
        break;
    }
}

// --- SetMissionDoesntRequireModel ---

GAME_DIFF_TEST(CStreaming6, SetMissionDoesntRequireModel_LoadedModel) {
    for (int32 id = 400; id < 500; id++) {
        auto& info = CStreaming::ms_aInfoForModel[id];
        if (!info.IsLoaded()) continue;

        auto saved = info.GetFlags();

        { HookDisableGuard guard("Global/CStreaming/SetMissionDoesntRequireModel");
          CStreaming::SetMissionDoesntRequireModel(id); }
        auto origFlags = info.GetFlags();

        info.SetFlags(saved);
        CStreaming::SetMissionDoesntRequireModel(id);
        auto revFlags = info.GetFlags();

        EXPECT_EQ(origFlags, revFlags);
        info.SetFlags(saved);
        break;
    }
}

// --- AddToLoadedVehiclesList ---

GAME_DIFF_TEST(CStreaming6, AddToLoadedVehiclesList_NoCrash) {
    { HookDisableGuard guard("Global/CStreaming/AddToLoadedVehiclesList");
      CStreaming::AddToLoadedVehiclesList(596); }
    CStreaming::AddToLoadedVehiclesList(596);
    EXPECT_TRUE(true);
}
