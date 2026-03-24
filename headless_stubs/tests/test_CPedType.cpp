// test_CPedType.cpp — Differential tests for CPedType.
// Hook paths: Global/CPedType/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PedType.h"

GAME_DIFF_TEST(CPedType, FindPedType) {
    const char* names[] = {
        "PLAYER1", "PLAYER2", "CIVMALE", "CIVFEMALE", "COP",
        "GANG1", "GANG2", "GANG3", "DEALER", "MEDIC", "FIREMAN",
        "CRIMINAL", "PROSTITUTE", "INVALID_TYPE_XYZ"
    };
    for (auto name : names) {
        int orig, rev;
        { HookDisableGuard guard("Global/CPedType/FindPedType");
          orig = (int)CPedType::FindPedType(name); }
        rev = (int)CPedType::FindPedType(name);
        if (orig != rev) {
            char buf[128];
            snprintf(buf, sizeof(buf), "FindPedType(\"%s\"): orig=%d rev=%d", name, orig, rev);
            GetTestContext().RecordFailure("test_CPedType.cpp", __LINE__, buf);
            return;
        }
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPedType, GetPedFlag) {
    // Test all valid ped types (0-31)
    for (int t = 0; t < 32; t++) {
        uint32 orig, rev;
        { HookDisableGuard guard("Global/CPedType/GetPedFlag");
          orig = CPedType::GetPedFlag(static_cast<ePedType>(t)); }
        rev = CPedType::GetPedFlag(static_cast<ePedType>(t));
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CPedType, PoliceDontCareAboutCrimesAgainstPedType) {
    for (int t = 0; t < 32; t++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPedType/PoliceDontCareAboutCrimesAgainstPedType");
          orig = CPedType::PoliceDontCareAboutCrimesAgainstPedType(static_cast<ePedType>(t)); }
        rev = CPedType::PoliceDontCareAboutCrimesAgainstPedType(static_cast<ePedType>(t));
        EXPECT_EQ(orig, rev);
    }
}

// --- GetPedTypeAcquaintances FULL SWEEP (32 types × 5 acquaintance IDs = 160) ---

GAME_DIFF_TEST(CPedType, GetPedTypeAcquaintances_FullSweep) {
    for (int32 type = 0; type < 32; type++) {
        for (int32 acqId = 0; acqId < 5; acqId++) {
            uint32 orig, rev;
            { HookDisableGuard guard("Global/CPedType/GetPedTypeAcquaintances-uint32");
              orig = CPedType::GetPedTypeAcquaintances(acqId, static_cast<ePedType>(type)); }
            rev = CPedType::GetPedTypeAcquaintances(acqId, static_cast<ePedType>(type));
            EXPECT_EQ(orig, rev);
        }
    }
}
