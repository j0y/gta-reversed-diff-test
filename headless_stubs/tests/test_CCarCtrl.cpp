// test_CCarCtrl.cpp — Differential tests for CCarCtrl.
// Hook paths: Global/CCarCtrl/<fn>

#include "StdInc.h"
#include "TestFramework.h"

GAME_DIFF_TEST(CCarCtrl, ChooseBoatModel) {
    // RNG-dependent — seed first for determinism
    srand(42);
    int32 orig, rev;
    { HookDisableGuard guard("Global/CCarCtrl/ChooseBoatModel"); srand(42); orig = CCarCtrl::ChooseBoatModel(); }
    srand(42);
    rev = CCarCtrl::ChooseBoatModel();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CCarCtrl, ChoosePoliceCarModel) {
    uint32 args[] = { 0, 1, 2, 3 };
    for (uint32 a : args) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CCarCtrl/ChoosePoliceCarModel"); orig = CCarCtrl::ChoosePoliceCarModel(a); }
        rev = CCarCtrl::ChoosePoliceCarModel(a);
        EXPECT_EQ(orig, rev);
    }
}

GAME_DIFF_TEST(CCarCtrl, ChooseGangCarModel) {
    for (int gang = 0; gang < 10; gang++) {
        eModelID orig, rev;
        { HookDisableGuard guard("Global/CCarCtrl/ChooseGangCarModel"); orig = CCarCtrl::ChooseGangCarModel((eGangID)gang); }
        rev = CCarCtrl::ChooseGangCarModel((eGangID)gang);
        EXPECT_EQ((int)orig, (int)rev);
    }
}
