// test_CMenuSystem.cpp — Differential tests for CMenuSystem.
// Hook paths: Global/CMenuSystem/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "MenuSystem.h"

// --- CheckForAccept ---
// Checks if a menu has been accepted (player pressed enter).

GAME_DIFF_TEST(CMenuSystem, CheckForAccept) {
    // Test all 4 menu slots (typically 0-3)
    for (uint8 menuId = 0; menuId < 4; menuId++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CMenuSystem/CheckForAccept");
          orig = CMenuSystem::CheckForAccept(menuId); }
        rev = CMenuSystem::CheckForAccept(menuId);
        EXPECT_EQ(orig, rev);
    }
}

// --- CheckForSelected ---

GAME_DIFF_TEST(CMenuSystem, CheckForSelected) {
    for (uint8 menuId = 0; menuId < 4; menuId++) {
        int8 orig, rev;
        { HookDisableGuard guard("Global/CMenuSystem/CheckForSelected");
          orig = CMenuSystem::CheckForSelected(menuId); }
        rev = CMenuSystem::CheckForSelected(menuId);
        EXPECT_EQ(orig, rev);
    }
}

// --- SwitchOffMenu ---

GAME_DIFF_TEST(CMenuSystem, SwitchOffMenu) {
    // Save menu state, switch off, compare
    for (uint8 menuId = 0; menuId < 4; menuId++) {
        auto& menu = CMenuSystem::MenuInUse[menuId];
        auto saved = menu;

        { HookDisableGuard guard("Global/CMenuSystem/SwitchOffMenu");
          CMenuSystem::SwitchOffMenu(menuId); }
        auto origState = menu;

        menu = saved;
        CMenuSystem::SwitchOffMenu(menuId);
        auto revState = menu;

        EXPECT_EQ(origState, revState);

        menu = saved;
    }
}
