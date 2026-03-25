// test_CShopping.cpp — Deep differential tests for CShopping.
// Hook paths: Global/CShopping/<fn>
//
// CShopping manages buyable items (clothes, tattoos, haircuts, etc.)
// Pure query functions on static data — ideal for differential testing.

#include "StdInc.h"
#include "TestFramework.h"
#include "Shopping.h"

// --- GetKey: convert model name + section to hash key ---

GAME_DIFF_TEST(CShopping, GetKey_ClothesNames) {
    const char* names[] = { "TSHIRT", "JEANS", "SNEAKER", "SHADES", "WATCH", "INVALID_XYZ" };
    for (auto* name : names) {
        for (int32 sec = 0; sec < 10; sec++) {
            uint32 orig, rev;
            { HookDisableGuard guard("Global/CShopping/GetKey");
              orig = CShopping::GetKey(name, static_cast<ePriceSection>(sec)); }
            rev = CShopping::GetKey(name, static_cast<ePriceSection>(sec));
            EXPECT_EQ(orig, rev);
        }
    }
}

// --- GetPriceSectionFromName ---

GAME_DIFF_TEST(CShopping, GetPriceSectionFromName_Various) {
    const char* names[] = { "CLOTHES", "TATTOOS", "HAIRCUTS", "FOOD" }; // "INVALID_SECTION" crashes original
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CShopping/GetPriceSectionFromName");
          orig = (int32)CShopping::GetPriceSectionFromName(name); }
        rev = (int32)CShopping::GetPriceSectionFromName(name);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindItem ---

GAME_DIFF_TEST(CShopping, FindItem_Various) {
    // Use hash keys of known items — test with some valid and invalid keys
    uint32 keys[] = { 0, 1, 100, 1000, 0xFFFFFFFF, 0xDEADBEEF };
    for (uint32 k : keys) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CShopping/FindItem"); orig = CShopping::FindItem(k); }
        rev = CShopping::FindItem(k);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetItemIndex ---

GAME_DIFF_TEST(CShopping, GetItemIndex_Various) {
    uint32 keys[] = { 0, 1, 100, 1000, 0xFFFFFFFF };
    for (uint32 k : keys) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CShopping/GetItemIndex"); orig = CShopping::GetItemIndex(k); }
        rev = CShopping::GetItemIndex(k);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetPrice ---

GAME_DIFF_TEST(CShopping, GetPrice_Various) {
    uint32 ids[] = { 0, 1, 5, 10, 50, 100 };
    for (uint32 id : ids) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CShopping/GetPrice"); orig = CShopping::GetPrice(id); }
        rev = CShopping::GetPrice(id);
        EXPECT_EQ(orig, rev);
    }
}

// --- HasPlayerBought ---

GAME_DIFF_TEST(CShopping, HasPlayerBought_Various) {
    uint32 keys[] = { 0, 1, 100, 1000, 0xFFFFFFFF };
    for (uint32 k : keys) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CShopping/HasPlayerBought"); orig = CShopping::HasPlayerBought(k); }
        rev = CShopping::HasPlayerBought(k);
        EXPECT_EQ(orig, rev);
    }
}

// --- GetExtraInfo ---

GAME_DIFF_TEST(CShopping, GetExtraInfo_Various) {
    uint32 keys[] = { 0, 1, 100 };
    for (uint32 k : keys) {
        for (int32 idx = 0; idx < 4; idx++) {
            int32 orig, rev;
            { HookDisableGuard guard("Global/CShopping/GetExtraInfo"); orig = CShopping::GetExtraInfo(k, idx); }
            rev = CShopping::GetExtraInfo(k, idx);
            EXPECT_EQ(orig, rev);
        }
    }
}
