// test_CDirectory.cpp — Differential tests for CDirectory.
// Hook paths: Global/CDirectory/<fn>
//
// Array-based file directory: Init, AddItem, FindItem (by name and hash).
// Allocated on stack with fixed capacity for tests.

#include "StdInc.h"
#include "TestFramework.h"
#include "Directory.h"
#include "KeyGen.h"

// Helper: create a directory with some test entries
static CDirectory MakeTestDir() {
    CDirectory dir(32);
    CDirectory::DirectoryInfo info;
    memset(&info, 0, sizeof(info));

    strcpy_s(info.Name, "vehicle.txd");
    info.Pos.Offset = 100; info.Size = 5000;
    dir.AddItem(info);

    strcpy_s(info.Name, "player.dff");
    info.Pos.Offset = 200; info.Size = 3000;
    dir.AddItem(info);

    strcpy_s(info.Name, "weapon.dat");
    info.Pos.Offset = 300; info.Size = 1000;
    dir.AddItem(info);

    return dir;
}

// --- FindItem (by name) ---

GAME_DIFF_TEST(CDirectory, FindItem_ByName) {
    CDirectory dir = MakeTestDir();

    const char* names[] = { "vehicle.txd", "player.dff", "weapon.dat", "VEHICLE.TXD", "notfound.abc" };
    for (const char* name : names) {
        CDirectory::DirectoryInfo* orig;
        CDirectory::DirectoryInfo* rev;
        { HookDisableGuard guard("Global/CDirectory/FindItem-");
          orig = dir.FindItem(name); }
        rev = dir.FindItem(name);
        EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
    }
}

// --- FindItem (by name, returning pos/size) ---

GAME_DIFF_TEST(CDirectory, FindItem_ByNamePosSize) {
    CDirectory dir = MakeTestDir();

    const char* names[] = { "vehicle.txd", "notfound.abc", "weapon.dat" };
    for (const char* name : names) {
        CdStreamPos origPos{}, revPos{};
        uint32 origSize = 0, revSize = 0;
        bool origFound, revFound;
        { HookDisableGuard guard("Global/CDirectory/FindItem-ByName");
          origFound = dir.FindItem(name, origPos, origSize); }
        revFound = dir.FindItem(name, revPos, revSize);
        EXPECT_EQ(origFound, revFound);
        if (origFound && revFound) {
            EXPECT_EQ(origPos.Offset, revPos.Offset);
            EXPECT_EQ(origSize, revSize);
        }
    }
}

// --- FindItem (by hash) ---

GAME_DIFF_TEST(CDirectory, FindItem_ByHash) {
    CDirectory dir = MakeTestDir();

    const char* names[] = { "vehicle.txd", "player.dff", "notfound.abc" };
    for (const char* name : names) {
        uint32 hash = CKeyGen::GetUppercaseKey(name);
        CdStreamPos origPos{}, revPos{};
        uint32 origSize = 0, revSize = 0;
        bool origFound, revFound;
        { HookDisableGuard guard("Global/CDirectory/FindItem-ByHash");
          origFound = dir.FindItem(hash, origPos, origSize); }
        revFound = dir.FindItem(hash, revPos, revSize);
        EXPECT_EQ(origFound, revFound);
        if (origFound && revFound) {
            EXPECT_EQ(origPos.Offset, revPos.Offset);
            EXPECT_EQ(origSize, revSize);
        }
    }
}

// --- AddItem ---

GAME_DIFF_TEST(CDirectory, AddItem) {
    CDirectory orig(16), rev(16);
    CDirectory::DirectoryInfo info;
    memset(&info, 0, sizeof(info));
    strcpy_s(info.Name, "test.txd");
    info.Pos.Offset = 42; info.Size = 999;

    { HookDisableGuard guard("Global/CDirectory/AddItem-");
      orig.AddItem(info); }
    rev.AddItem(info);

    // Both should be findable
    auto* origEntry = orig.FindItem("test.txd");
    auto* revEntry = rev.FindItem("test.txd");
    EXPECT_TRUE(origEntry != nullptr);
    EXPECT_TRUE(revEntry != nullptr);
    if (origEntry && revEntry) {
        EXPECT_EQ(origEntry->Pos.Offset, revEntry->Pos.Offset);
        EXPECT_EQ(origEntry->Size, revEntry->Size);
    }
}
