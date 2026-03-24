// test_CControllerConfigManager3.cpp — Extended differential tests.
// Hook paths: Global/CControllerConfigManager/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "ControllerConfigManager.h"

// --- GetActionType ---

GAME_DIFF_TEST(CControllerConfigManager3, GetActionType_AllActions) {
    for (int32 action = 0; action < 60; action++) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetActionType");
          orig = (int32)ControlsManager.GetActionType(static_cast<eControllerAction>(action)); }
        rev = (int32)ControlsManager.GetActionType(static_cast<eControllerAction>(action));
        EXPECT_EQ(orig, rev);
    }
}

// --- GetIsActionAButtonCombo ---

GAME_DIFF_TEST(CControllerConfigManager3, GetIsActionAButtonCombo_AllActions) {
    for (int32 action = 0; action < 60; action++) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CControllerConfigManager/GetIsActionAButtonCombo");
          orig = ControlsManager.GetIsActionAButtonCombo(static_cast<eControllerAction>(action)); }
        rev = ControlsManager.GetIsActionAButtonCombo(static_cast<eControllerAction>(action));
        EXPECT_EQ(orig, rev);
    }
}

// --- GetControllerKeyAssociatedWithAction ---

GAME_DIFF_TEST(CControllerConfigManager3, GetControllerKeyAssociatedWithAction_Sample) {
    int32 actions[] = { 0, 1, 2, 3, 5, 10, 15, 20, 30 };
    for (int32 action : actions) {
        for (int32 keyType = 0; keyType < 2; keyType++) {
            int32 orig, rev;
            { HookDisableGuard guard("Global/CControllerConfigManager/GetControllerKeyAssociatedWithAction");
              orig = (int32)ControlsManager.GetControllerKeyAssociatedWithAction(
                  static_cast<eControllerAction>(action), static_cast<eControllerType>(keyType)); }
            rev = (int32)ControlsManager.GetControllerKeyAssociatedWithAction(
                  static_cast<eControllerAction>(action), static_cast<eControllerType>(keyType));
            EXPECT_EQ(orig, rev);
        }
    }
}
