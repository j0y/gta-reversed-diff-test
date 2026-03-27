// test_CStreamingInfo.cpp — Differential tests for CStreamingInfo.
// Hook paths: Global/CStreamingInfo/<fn>
//
// SetCdPosnAndSize/GetCdPosnAndSize/GetCdPosn/InList/Init are safe —
// they operate on member fields. AddToList/RemoveFromList use asserts
// and need ms_pArrayBase set up — use real streaming entries instead.

#include "StdInc.h"
#include "TestFramework.h"
#include "StreamingInfo.h"
#include "Streaming.h"

// --- Init ---

GAME_DIFF_TEST(CStreamingInfo, Init) {
    CStreamingInfo orig{}, rev{};
    memset(&orig, 0xAB, sizeof(orig));
    memset(&rev, 0xAB, sizeof(rev));
    { HookDisableGuard guard("Global/CStreamingInfo/Init");
      orig.Init(); }
    rev.Init();
    // Compare key fields (avoid padding byte diffs)
    EXPECT_EQ(orig.m_NextIndex, rev.m_NextIndex);
    EXPECT_EQ(orig.m_PrevIndex, rev.m_PrevIndex);
    EXPECT_EQ(orig.m_CDOffset, rev.m_CDOffset);
    EXPECT_EQ(orig.m_CDSize, rev.m_CDSize);
    EXPECT_EQ(orig.m_ImgID, rev.m_ImgID);
}

// --- SetCdPosnAndSize + GetCdPosnAndSize ---

GAME_DIFF_TEST(CStreamingInfo, SetAndGetCdPosnAndSize) {
    struct Case { uint32 offset; size_t size; };
    Case cases[] = { {0, 0}, {100, 5000}, {0xFFFFF, 1}, {42, 999} };

    for (auto& c : cases) {
        CStreamingInfo orig{}, rev{};
        orig.Init(); rev.Init();

        { HookDisableGuard guard("Global/CStreamingInfo/SetCdPosnAndSize");
          orig.SetCdPosnAndSize(c.offset, c.size); }
        rev.SetCdPosnAndSize(c.offset, c.size);

        CdStreamPos origPos{}, revPos{};
        size_t origSize = 0, revSize = 0;
        bool origOk, revOk;

        { HookDisableGuard guard("Global/CStreamingInfo/GetCdPosnAndSize");
          origOk = orig.GetCdPosnAndSize(origPos, origSize); }
        revOk = rev.GetCdPosnAndSize(revPos, revSize);

        EXPECT_EQ(origOk, revOk);
        EXPECT_EQ(origSize, revSize);
    }
}

// --- InList ---

GAME_DIFF_TEST(CStreamingInfo, InList) {
    // Fresh init → not in list
    CStreamingInfo info{};
    info.Init();
    bool orig, rev;
    { HookDisableGuard guard("Global/CStreamingInfo/InList");
      orig = info.InList(); }
    rev = info.InList();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(rev, false);

    // Set next/prev to simulate being in a list
    info.m_NextIndex = 5;
    info.m_PrevIndex = 3;
    { HookDisableGuard guard("Global/CStreamingInfo/InList");
      orig = info.InList(); }
    rev = info.InList();
    EXPECT_EQ(orig, rev);
    EXPECT_EQ(rev, true);

    // BUG CHECK: Only m_NextIndex set, m_PrevIndex=-1
    // Original only checks m_NextIndex != -1.
    // Reversed adds extra m_PrevIndex != -1 check (NOTSA comment in source).
    info.m_NextIndex = 5;
    info.m_PrevIndex = -1;
    { HookDisableGuard guard("Global/CStreamingInfo/InList");
      orig = info.InList(); }
    rev = info.InList();
    // orig=true, rev=false — reversed has stricter check
    EXPECT_EQ(orig, rev);
}
