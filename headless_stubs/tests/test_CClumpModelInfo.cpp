// test_CClumpModelInfo.cpp — Differential tests for CClumpModelInfo.
// Hook paths: Models/CClumpModelInfo/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Models/ClumpModelInfo.h"
#include "ModelInfo.h"

// Helper: get a loaded CClumpModelInfo (ped models are clumps)
static CClumpModelInfo* GetLoadedClumpModelInfo() {
    auto* info = CModelInfo::GetModelInfo(0); // model 0 is a ped
    if (info && info->GetModelType() == MODEL_INFO_PED)
        return static_cast<CClumpModelInfo*>(info);
    return nullptr;
}

GAME_DIFF_TEST(CClumpModelInfo, GetAnimFileIndex) {
    auto* mi = GetLoadedClumpModelInfo();
    if (!mi) return;
    int32 orig, rev;
    { HookDisableGuard guard("Models/CClumpModelInfo/GetAnimFileIndex");
      orig = mi->GetAnimFileIndex(); }
    rev = mi->GetAnimFileIndex();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CClumpModelInfo, GetModelType) {
    auto* mi = GetLoadedClumpModelInfo();
    if (!mi) return;
    int32 orig, rev;
    { HookDisableGuard guard("Models/CClumpModelInfo/GetModelType");
      orig = (int32)mi->GetModelType(); }
    rev = (int32)mi->GetModelType();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CClumpModelInfo, GetRwModelType) {
    auto* mi = GetLoadedClumpModelInfo();
    if (!mi) return;
    int32 orig, rev;
    { HookDisableGuard guard("Models/CClumpModelInfo/GetRwModelType");
      orig = (int32)mi->GetRwModelType(); }
    rev = (int32)mi->GetRwModelType();
    EXPECT_EQ(orig, rev);
}

// Test across multiple loaded models
GAME_DIFF_TEST(CClumpModelInfo, GetAnimFileIndex_MultipleModels) {
    int32 checked = 0;
    for (int32 id = 0; id < 312 && checked < 10; id++) {
        auto* info = CModelInfo::GetModelInfo(id);
        if (!info || info->GetModelType() != MODEL_INFO_PED) continue;
        auto* mi = static_cast<CClumpModelInfo*>(info);
        int32 orig, rev;
        { HookDisableGuard guard("Models/CClumpModelInfo/GetAnimFileIndex");
          orig = mi->GetAnimFileIndex(); }
        rev = mi->GetAnimFileIndex();
        EXPECT_EQ(orig, rev);
        checked++;
    }
}
