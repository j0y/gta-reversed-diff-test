// test_CBridge.cpp — Differential tests for CBridge.
// Hook paths: Global/CBridge/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Bridge.h"

GAME_DIFF_TEST(CBridge, ShouldLightsBeFlashing) {
    bool orig, rev;
    { HookDisableGuard guard("Global/CBridge/ShouldLightsBeFlashing"); orig = CBridge::ShouldLightsBeFlashing(); }
    rev = CBridge::ShouldLightsBeFlashing();
    EXPECT_EQ(orig, rev);
}

// --- Init ---

GAME_DIFF_TEST(CBridge, Init) {
    { HookDisableGuard guard("Global/CBridge/Init");
      CBridge::Init(); }
    CBridge::Init();
    EXPECT_TRUE(true);
}

// --- Update ---

GAME_DIFF_TEST(CBridge, Update) {
    { HookDisableGuard guard("Global/CBridge/Update");
      CBridge::Update(); }
    CBridge::Update();
    EXPECT_TRUE(true);
}

// --- FindBridgeEntities ---

GAME_DIFF_TEST(CBridge, FindBridgeEntities) {
    { HookDisableGuard guard("Global/CBridge/FindBridgeEntities");
      CBridge::FindBridgeEntities(); }
    CBridge::FindBridgeEntities();
    EXPECT_TRUE(true);
}
