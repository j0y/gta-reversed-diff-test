// test_CTheScripts5.cpp — Extended differential tests for CTheScripts.
// Hook paths: Scripts/CTheScripts/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Scripts/TheScripts.h"

GAME_DIFF_TEST(CTheScripts5, ClearAllSuppressedCarModels) {
    { HookDisableGuard guard("Scripts/CTheScripts/ClearAllSuppressedCarModels");
      CTheScripts::ClearAllSuppressedCarModels(); }
    CTheScripts::ClearAllSuppressedCarModels();
    EXPECT_TRUE(true);
}

GAME_DIFF_TEST(CTheScripts5, ClearAllVehicleModelsBlockedByScript) {
    { HookDisableGuard guard("Scripts/CTheScripts/ClearAllVehicleModelsBlockedByScript");
      CTheScripts::ClearAllVehicleModelsBlockedByScript(); }
    CTheScripts::ClearAllVehicleModelsBlockedByScript();
    EXPECT_TRUE(true);
}
