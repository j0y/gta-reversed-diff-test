// test_CLoadedCarGroup.cpp — Differential tests for CLoadedCarGroup.
// Hook paths: Global/CLoadedCarGroup/<fn>
//
// Array-based car model group with Clear, Add, Remove, Count, GetMember.
// Uses MODEL_INVALID (-1) as sentinel. Works on 'this' member array.

#include "StdInc.h"
#include "TestFramework.h"
#include "LoadedCarGroup.h"

// --- Clear ---

GAME_DIFF_TEST(CLoadedCarGroup, Clear) {
    CLoadedCarGroup orig{}, rev{};
    // Dirty them first
    memset(&orig, 0x42, sizeof(orig));
    memset(&rev, 0x42, sizeof(rev));
    { HookDisableGuard guard("Global/CLoadedCarGroup/Clear");
      orig.Clear(); }
    rev.Clear();
    EXPECT_EQ(orig.CountMembers(), rev.CountMembers());
    EXPECT_EQ(orig.CountMembers(), 0);
}

// --- AddMember + CountMembers ---

GAME_DIFF_TEST(CLoadedCarGroup, AddMemberAndCount) {
    CLoadedCarGroup orig{}, rev{};
    orig.Clear(); rev.Clear();

    eModelID models[] = { (eModelID)400, (eModelID)401, (eModelID)402, (eModelID)500, (eModelID)596 };
    for (eModelID m : models) {
        { HookDisableGuard guard("Global/CLoadedCarGroup/AddMember");
          orig.AddMember(m); }
        rev.AddMember(m);
    }

    int32 origCount, revCount;
    { HookDisableGuard guard("Global/CLoadedCarGroup/CountMembers");
      origCount = orig.CountMembers(); }
    revCount = rev.CountMembers();
    EXPECT_EQ(origCount, revCount);
    EXPECT_EQ(revCount, 5);
}

// --- GetMember ---

GAME_DIFF_TEST(CLoadedCarGroup, GetMember) {
    CLoadedCarGroup orig{}, rev{};
    orig.Clear(); rev.Clear();
    orig.AddMember((eModelID)400); orig.AddMember((eModelID)500); orig.AddMember((eModelID)596);
    rev.AddMember((eModelID)400); rev.AddMember((eModelID)500); rev.AddMember((eModelID)596);

    for (int32 i = 0; i < 3; i++) {
        int32 origM, revM;
        { HookDisableGuard guard("Global/CLoadedCarGroup/GetMember");
          origM = orig.GetMember(i); }
        revM = rev.GetMember(i);
        EXPECT_EQ(origM, revM);
    }
}

// --- RemoveMember ---

GAME_DIFF_TEST(CLoadedCarGroup, RemoveMember) {
    CLoadedCarGroup orig{}, rev{};
    orig.Clear(); rev.Clear();
    orig.AddMember((eModelID)400); orig.AddMember((eModelID)500); orig.AddMember((eModelID)596);
    rev.AddMember((eModelID)400); rev.AddMember((eModelID)500); rev.AddMember((eModelID)596);

    // Remove middle element
    { HookDisableGuard guard("Global/CLoadedCarGroup/RemoveMember");
      orig.RemoveMember((eModelID)500); }
    rev.RemoveMember((eModelID)500);

    EXPECT_EQ(orig.CountMembers(), rev.CountMembers());
    EXPECT_EQ(rev.CountMembers(), 2);

    // Verify remaining members match
    for (int32 i = 0; i < 2; i++) {
        EXPECT_EQ(orig.GetMember(i), rev.GetMember(i));
    }
}

// --- Behavior tests ---

GAME_TEST(CLoadedCarGroup, AddRemoveCycle) {
    CLoadedCarGroup g{};
    g.Clear();
    EXPECT_EQ(g.CountMembers(), 0);
    g.AddMember((eModelID)400);
    g.AddMember((eModelID)401);
    EXPECT_EQ(g.CountMembers(), 2);
    g.RemoveMember((eModelID)400);
    EXPECT_EQ(g.CountMembers(), 1);
    EXPECT_EQ(g.GetMember(0), 401);
}
