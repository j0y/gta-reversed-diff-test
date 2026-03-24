// test_Scenario_Streaming2.cpp — Advanced streaming sequence tests.
//
// Tests RequestModel -> LoadAllRequestedModels -> query sequences.

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"
#include "ModelInfo.h"
#include "ScenarioHelpers.h"

// --- Request + Load + Check round-trip for TXD ---

GAME_TEST(Scenario_Streaming2, RequestTxd_LoadAndCheck) {
    int32 txdToLoad = -1;
    for (int32 i = 20000; i < 20100; i++) {
        auto& info = CStreaming::ms_aInfoForModel[i];
        if (info.IsLoaded()) continue;
        if (info.GetCdSize() == 0) continue;
        txdToLoad = i;
        break;
    }
    if (txdToLoad < 0) return;

    CStreaming::RequestModel(txdToLoad, STREAMING_KEEP_IN_MEMORY);

    {
        ResumeOtherThreads threads;
        CStreaming::LoadAllRequestedModels(false);
    }

    bool loaded = CStreaming::ms_aInfoForModel[txdToLoad].IsLoaded();
    EXPECT_TRUE(loaded);

    if (loaded) {
        CStreaming::SetModelIsDeletable(txdToLoad);
        CStreaming::RemoveModel(txdToLoad);
    }
}

// --- Verify model info consistency after streaming ---

GAME_TEST(Scenario_Streaming2, StreamedModelInfo_Consistency) {
    int32 checked = 0;
    for (int32 id = 400; id < 612; id++) {
        auto& info = CStreaming::ms_aInfoForModel[id];
        if (!info.IsLoaded()) continue;

        auto* modelInfo = CModelInfo::GetModelInfo(id);
        EXPECT_TRUE(modelInfo != nullptr);
        if (modelInfo) {
            EXPECT_TRUE(modelInfo->GetModelType() == MODEL_INFO_VEHICLE);
        }
        checked++;
        if (checked >= 5) break;
    }
    EXPECT_TRUE(checked > 0);
}
