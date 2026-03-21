// test_CControllerConfigManager.cpp — Differential tests for CControllerConfigManager.
// Hook paths: Global/CControllerConfigManager/<fn>
// ControlsManager is a global instance at state 9.

#include "StdInc.h"
#include "TestFramework.h"
#include "ControllerConfigManager.h"

// --- Action type queries (enum-indexed lookups) ---

GAME_DIFF_TEST(CControllerConfigManager, GetActionType) {
    // Test a range of controller actions
    for (int a = 0; a < 45; a++) {
        auto action = static_cast<eControllerAction>(a);
        int orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetActionType");
          orig = (int)ControlsManager.GetActionType(action); }
        rev = (int)ControlsManager.GetActionType(action);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CControllerConfigManager, GetNumOfSettingsForAction) {
    for (int a = 0; a < 45; a++) {
        auto action = static_cast<eControllerAction>(a);
        int orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetNumOfSettingsForAction");
          orig = (int)ControlsManager.GetNumOfSettingsForAction(action); }
        rev = (int)ControlsManager.GetNumOfSettingsForAction(action);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CControllerConfigManager, GetIsActionAButtonCombo) {
    for (int a = 0; a < 45; a++) {
        auto action = static_cast<eControllerAction>(a);
        bool orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetIsActionAButtonCombo");
          orig = ControlsManager.GetIsActionAButtonCombo(action); }
        rev = ControlsManager.GetIsActionAButtonCombo(action);
        EXPECT_EQ(orig, rev);
    }
}

// --- Key/mouse binding queries ---

GAME_DIFF_TEST(CControllerConfigManager, GetControllerKeyAssociatedWithAction) {
    // Test first 20 actions across keyboard and joypad types
    for (int a = 0; a < 20; a++) {
        auto action = static_cast<eControllerAction>(a);
        for (int t = 0; t < 4; t++) {
            auto type = static_cast<eControllerType>(t);
            int orig, rev;
            { HookDisableGuard guard("Global/CControllerConfigManager/GetControllerKeyAssociatedWithAction");
              orig = (int)ControlsManager.GetControllerKeyAssociatedWithAction(action, type); }
            rev = (int)ControlsManager.GetControllerKeyAssociatedWithAction(action, type);
            EXPECT_EQ(orig, rev);
        }
    }
}

GAME_DIFF_TEST(CControllerConfigManager, GetMouseButtonAssociatedWithAction) {
    for (int a = 0; a < 20; a++) {
        auto action = static_cast<eControllerAction>(a);
        int orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetMouseButtonAssociatedWithAction");
          orig = (int)ControlsManager.GetMouseButtonAssociatedWithAction(action); }
        rev = (int)ControlsManager.GetMouseButtonAssociatedWithAction(action);
        EXPECT_EQ(orig, rev);
    }
}

// --- Input state queries (test with known key codes) ---

GAME_DIFF_TEST(CControllerConfigManager, GetIsKeyboardKeyDown) {
    // Test common key codes: 0 (none), A-Z range, arrow keys
    int keys[] = { 0, 1, 13, 27, 32, 65, 66, 87, 83, 68, 37, 38, 39, 40, 255 };
    for (int k : keys) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetIsKeyboardKeyDown");
          orig = ControlsManager.GetIsKeyboardKeyDown((KeyCode)k); }
        rev = ControlsManager.GetIsKeyboardKeyDown((KeyCode)k);
        EXPECT_EQ(orig, rev);
    }
}

// GetIsKeyboardKeyJustDown is protected — skipped

GAME_DIFF_TEST(CControllerConfigManager, GetIsMouseButtonDown) {
    // Mouse buttons 0-4
    for (int b = 0; b < 5; b++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetIsMouseButtonDown");
          orig = ControlsManager.GetIsMouseButtonDown((KeyCode)b); }
        rev = ControlsManager.GetIsMouseButtonDown((KeyCode)b);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CControllerConfigManager, GetIsMouseButtonUp) {
    for (int b = 0; b < 5; b++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetIsMouseButtonUp");
          orig = ControlsManager.GetIsMouseButtonUp((KeyCode)b); }
        rev = ControlsManager.GetIsMouseButtonUp((KeyCode)b);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CControllerConfigManager, GetIsMouseButtonJustUp) {
    for (int b = 0; b < 5; b++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetIsMouseButtonJustUp");
          orig = ControlsManager.GetIsMouseButtonJustUp((KeyCode)b); }
        rev = ControlsManager.GetIsMouseButtonJustUp((KeyCode)b);
        EXPECT_EQ(orig, rev);
    }
}

// GetIsKeyBlank is protected — skipped
