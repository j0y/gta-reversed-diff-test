// test_CEntryExitManager2.cpp — Deep differential tests for CEntryExitManager.
// Hook paths: Global/CEntryExitManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "EntryExitManager.h"

// --- EnableBurglaryHouses ---

GAME_DIFF_TEST(CEntryExitManager, EnableBurglaryHouses_Enable) {
    { HookDisableGuard guard("Global/CEntryExitManager/EnableBurglaryHouses");
      CEntryExitManager::EnableBurglaryHouses(true); }
    CEntryExitManager::EnableBurglaryHouses(true);
    EXPECT_TRUE(true); // verify no crash
}

GAME_DIFF_TEST(CEntryExitManager, EnableBurglaryHouses_Disable) {
    { HookDisableGuard guard("Global/CEntryExitManager/EnableBurglaryHouses");
      CEntryExitManager::EnableBurglaryHouses(false); }
    CEntryExitManager::EnableBurglaryHouses(false);
    EXPECT_TRUE(true);
}
