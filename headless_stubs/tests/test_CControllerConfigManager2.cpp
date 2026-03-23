// test_CControllerConfigManager2.cpp — Deep differential tests.
// Hook paths: Global/CControllerConfigManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "ControllerConfigManager.h"

// --- GetControllerKeyAssociatedWithAction ---

GAME_DIFF_TEST(CControllerConfigManager2, GetControllerKeyAssociatedWithAction_Keyboard) {
    // Test common actions with keyboard controller type
    for (int32 action = 0; action < 20; action++) {
        uint32 orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetControllerKeyAssociatedWithAction");
          orig = (uint32)ControlsManager.GetControllerKeyAssociatedWithAction((eControllerAction)action, eControllerType::KEYBOARD); }
        rev = (uint32)ControlsManager.GetControllerKeyAssociatedWithAction((eControllerAction)action, eControllerType::KEYBOARD);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetIsKeyboardKeyDown ---

GAME_DIFF_TEST(CControllerConfigManager2, GetIsKeyboardKeyDown_Various) {
    // Test common key codes
    uint32 keys[] = { 0, 1, 13, 27, 32, 65, 87, 83, 68, 255 };
    for (uint32 k : keys) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetIsKeyboardKeyDown");
          orig = ControlsManager.GetIsKeyboardKeyDown((KeyCode)k); }
        rev = ControlsManager.GetIsKeyboardKeyDown((KeyCode)k);
        EXPECT_EQ(orig, rev);
    }
}

// GetIsKeyboardKeyJustDown is protected — skip

// --- GetIsMouseButtonDown ---

GAME_DIFF_TEST(CControllerConfigManager2, GetIsMouseButtonDown_Various) {
    uint32 buttons[] = { 0, 1, 2, 3, 4, 5 };
    for (uint32 b : buttons) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetIsMouseButtonDown");
          orig = ControlsManager.GetIsMouseButtonDown((KeyCode)b); }
        rev = ControlsManager.GetIsMouseButtonDown((KeyCode)b);
        EXPECT_EQ(orig, rev);
    }
}
