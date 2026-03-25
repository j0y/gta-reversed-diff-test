// test_InteriorManager2.cpp — Additional differential tests for InteriorManager_c.
// Hook paths: Interior/InteriorManager_c/<fn>
//
// Extends test_InteriorManager.cpp with tests for:
// - IsInteriorEffectVisible
// - HasInteriorHadStealDataSetup
// - GetBoundingBox
// - Update

#include "StdInc.h"
#include "TestFramework.h"
#include "Interior/InteriorManager_c.h"

// --- IsInteriorEffectVisible ---

// IsInteriorEffectVisible(nullptr, nullptr) crashes original code — skip

// --- HasInteriorHadStealDataSetup ---

GAME_DIFF_TEST(InteriorManager, HasInteriorHadStealDataSetup_Null) {
    bool orig, rev;
    { HookDisableGuard guard("Interior/InteriorManager_c/HasInteriorHadStealDataSetup");
      orig = g_interiorMan.HasInteriorHadStealDataSetup(nullptr); }
    rev = g_interiorMan.HasInteriorHadStealDataSetup(nullptr);
    EXPECT_EQ(orig, rev);
}

// --- GetBoundingBox ---

GAME_DIFF_TEST(InteriorManager, GetBoundingBox_NullArgs) {
    bool orig, rev;
    { HookDisableGuard guard("Interior/InteriorManager_c/GetBoundingBox");
      orig = g_interiorMan.GetBoundingBox(nullptr, nullptr); }
    rev = g_interiorMan.GetBoundingBox(nullptr, nullptr);
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(InteriorManager, GetBoundingBox_NullEntity) {
    CVector bbox{};
    bool orig, rev;
    { HookDisableGuard guard("Interior/InteriorManager_c/GetBoundingBox");
      orig = g_interiorMan.GetBoundingBox(nullptr, &bbox); }
    CVector bboxOrig = bbox;

    bbox = CVector(0.f, 0.f, 0.f);
    rev = g_interiorMan.GetBoundingBox(nullptr, &bbox);
    CVector bboxRev = bbox;

    EXPECT_EQ(orig, rev);
    EXPECT_NEAR(bboxOrig.x, bboxRev.x, 0.001f);
    EXPECT_NEAR(bboxOrig.y, bboxRev.y, 0.001f);
    EXPECT_NEAR(bboxOrig.z, bboxRev.z, 0.001f);
}

// --- Update ---

GAME_DIFF_TEST(InteriorManager, Update_Agreement) {
    // Update is stateful but should return the same bool from both paths
    // given identical game state. We call reversed first, then original.
    bool orig, rev;
    rev = g_interiorMan.Update();
    { HookDisableGuard guard("Interior/InteriorManager_c/Update");
      orig = g_interiorMan.Update(); }
    EXPECT_EQ(orig, rev);
}
