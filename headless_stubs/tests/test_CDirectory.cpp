// test_CDirectory.cpp — Differential tests for CDirectory.
// Hook paths: Global/CDirectory/<fn>
//
// CDirectory manages file lookup tables for IMG archives.
// CStreaming has a CDirectory we can query.

#include "StdInc.h"
#include "TestFramework.h"
#include "Streaming.h"

// --- FindItem by name ---

GAME_DIFF_TEST(CDirectory, FindItem_KnownFiles) {
    auto* dir = CStreaming::ms_pExtraObjectsDir;
    if (!dir) return;

    // Known file names that should exist in the IMG archives
    const char* names[] = {
        "cars.txd",
        "generic.txd",
        "particle.txd",
        "hud.txd",
        "fonts.txd",
        "NONEXISTENT_FILE.xyz",
    };
    for (const char* name : names) {
        auto* origItem = (CDirectory::DirectoryInfo*)nullptr;
        auto* revItem = (CDirectory::DirectoryInfo*)nullptr;
        { HookDisableGuard guard("Global/CDirectory/FindItem-");
          origItem = dir->FindItem(name); }
        revItem = dir->FindItem(name);
        // Both should find (or not find) the same entry
        EXPECT_EQ((origItem != nullptr), (revItem != nullptr));
        if (origItem && revItem) {
            EXPECT_EQ(origItem->Size, revItem->Size);
            EXPECT_EQ(strcmp(origItem->Name, revItem->Name), 0);
        }
    }
}

// --- FindItem by name with output params ---

GAME_DIFF_TEST(CDirectory, FindItem_WithOutputParams) {
    auto* dir = CStreaming::ms_pExtraObjectsDir;
    if (!dir) return;

    const char* names[] = { "cars.txd", "generic.txd", "NONEXISTENT.xyz" };
    for (const char* name : names) {
        CdStreamPos origPos{}, revPos{};
        uint32 origSize = 0, revSize = 0;
        bool origFound, revFound;
        { HookDisableGuard guard("Global/CDirectory/FindItem-ByName");
          origFound = dir->FindItem(name, origPos, origSize); }
        revFound = dir->FindItem(name, revPos, revSize);
        EXPECT_EQ(origFound, revFound);
        if (origFound && revFound) {
            EXPECT_EQ(origSize, revSize);
        }
    }
}
