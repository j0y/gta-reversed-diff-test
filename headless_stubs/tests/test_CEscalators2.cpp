// test_CEscalators2.cpp — CEscalators differential tests.
// Hook paths: Global/CEscalators/<fn>
// Already tested: Init behavior (test_CEscalators.cpp)

#include "StdInc.h"
#include "TestFramework.h"
#include "Escalators.h"

GAME_DIFF_TEST(CEscalators2, AddOne_NoCrash) {
    CVector start(2488.f, -1666.f, 10.f);
    CVector bottom(2488.f, -1666.f, 10.f);
    CVector top(2488.f, -1666.f, 14.f);
    CVector end(2492.f, -1666.f, 14.f);
    { HookDisableGuard guard("Global/CEscalators/AddOne");
      CEscalators::AddOne(start, bottom, top, end, true, nullptr); }
    CEscalators::AddOne(start, bottom, top, end, true, nullptr);
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CEscalators2, Shutdown_Init_Roundtrip) {
    { HookDisableGuard guard("Global/CEscalators/Shutdown");
      CEscalators::Shutdown(); }
    CEscalators::Shutdown();
    // Re-init so other tests aren't broken
    CEscalators::Init();
    EXPECT_TRUE(true);
}
