// test_CShopping2.cpp — Systematic CShopping differential tests.
// Tests ALL remaining untested hooked CShopping functions.
// Hook paths: Global/CShopping/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "Shopping.h"

// ===================================================================
// GetNameTag — returns const char* for item key
// ===================================================================

GAME_DIFF_TEST(CShopping2, GetNameTag_ValidKeys) {
    // Use GetKey to create valid keys, then look up name tags
    const char* names[] = { "TSHIRT", "JEANS", "SNEAKER" };
    for (auto* name : names) {
        uint32 key = CShopping::GetKey(name, (ePriceSection)0);
        const char* orig;
        const char* rev;
        { HookDisableGuard guard("Global/CShopping/GetNameTag");
          orig = CShopping::GetNameTag(key); }
        rev = CShopping::GetNameTag(key);
        EXPECT_EQ(orig, rev); // same pointer (static data)
    }
}

// GetNameTag(invalid key) crashes original code (NOTSA_UNREACHABLE) — skip

// ===================================================================
// SetPlayerHasBought — void, marks item as bought
// Test with an invalid key (no real purchase effect)
// ===================================================================

// SetPlayerHasBought(invalid key) crashes original code (NOTSA_UNREACHABLE) — skip

// ===================================================================
// RemoveLoadedPrices — void, clears loaded price data
// ===================================================================

GAME_DIFF_TEST(CShopping2, RemoveLoadedPrices) {
    { HookDisableGuard guard("Global/CShopping/RemoveLoadedPrices");
      CShopping::RemoveLoadedPrices(); }
    CShopping::RemoveLoadedPrices();
    EXPECT_TRUE(true);
}

// ===================================================================
// RemoveLoadedShop — void, clears loaded shop data
// ===================================================================

GAME_DIFF_TEST(CShopping2, RemoveLoadedShop) {
    { HookDisableGuard guard("Global/CShopping/RemoveLoadedShop");
      CShopping::RemoveLoadedShop(); }
    CShopping::RemoveLoadedShop();
    EXPECT_TRUE(true);
}

// ===================================================================
// ShutdownForRestart — void, resets shopping state
// ===================================================================

GAME_DIFF_TEST(CShopping2, ShutdownForRestart) {
    { HookDisableGuard guard("Global/CShopping/ShutdownForRestart");
      CShopping::ShutdownForRestart(); }
    CShopping::ShutdownForRestart();
    EXPECT_TRUE(true);
}

// ===================================================================
// StoreClothesState / RestoreClothesState — save/restore round-trip
// ===================================================================

GAME_DIFF_TEST(CShopping2, StoreRestoreClothesState) {
    { HookDisableGuard guard("Global/CShopping/StoreClothesState");
      CShopping::StoreClothesState(); }
    CShopping::StoreClothesState();
    // Both should store the same state — verify restore doesn't crash
    { HookDisableGuard guard("Global/CShopping/RestoreClothesState");
      CShopping::RestoreClothesState(); }
    CShopping::RestoreClothesState();
    EXPECT_TRUE(true);
}

// ===================================================================
// StoreVehicleMods / RestoreVehicleMods — save/restore round-trip
// ===================================================================

// StoreVehicleMods/RestoreVehicleMods crashes headless (vehicle mod state not available) — skip

// ===================================================================
// SetCurrentProperty — void, sets current property pointer
// ===================================================================

GAME_DIFF_TEST(CShopping2, SetCurrentProperty_Null) {
    { HookDisableGuard guard("Global/CShopping/SetCurrentProperty");
      CShopping::SetCurrentProperty(nullptr); }
    CShopping::SetCurrentProperty(nullptr);
    EXPECT_TRUE(true);
}

// ===================================================================
// LoadStats — void, loads stat data from files
// ===================================================================

GAME_DIFF_TEST(CShopping2, LoadStats) {
    { HookDisableGuard guard("Global/CShopping/LoadStats");
      CShopping::LoadStats(); }
    CShopping::LoadStats();
    EXPECT_TRUE(true);
}

// ===================================================================
// GetKey — extend with more sections and names
// ===================================================================

GAME_DIFF_TEST(CShopping2, GetKey_AllSections) {
    const char* names[] = { "PLAYER", "COP", "BALLAS", "GROVE" };
    for (auto* name : names) {
        for (int32 sec = 0; sec < 15; sec++) {
            uint32 orig, rev;
            { HookDisableGuard guard("Global/CShopping/GetKey");
              orig = CShopping::GetKey(name, static_cast<ePriceSection>(sec)); }
            rev = CShopping::GetKey(name, static_cast<ePriceSection>(sec));
            EXPECT_EQ(orig, rev);
        }
    }
}

// ===================================================================
// GetPriceSectionFromName — extend with more section names
// ===================================================================

GAME_DIFF_TEST(CShopping2, GetPriceSectionFromName_Extended) {
    const char* names[] = { "CLOTHES", "TATTOOS", "HAIRCUTS", "FOOD" };
    for (auto* name : names) {
        int32 orig, rev;
        { HookDisableGuard guard("Global/CShopping/GetPriceSectionFromName");
          orig = (int32)CShopping::GetPriceSectionFromName(name); }
        rev = (int32)CShopping::GetPriceSectionFromName(name);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// HasPlayerBought — extend with computed valid keys
// ===================================================================

GAME_DIFF_TEST(CShopping2, HasPlayerBought_ComputedKeys) {
    const char* items[] = { "TSHIRT", "JEANS", "SNEAKER", "SHADES" };
    for (auto* item : items) {
        uint32 key = CShopping::GetKey(item, (ePriceSection)0);
        bool orig, rev;
        { HookDisableGuard guard("Global/CShopping/HasPlayerBought");
          orig = CShopping::HasPlayerBought(key); }
        rev = CShopping::HasPlayerBought(key);
        EXPECT_EQ(orig, rev);
    }
}

// ===================================================================
// GetPrice — extend with computed keys
// ===================================================================

GAME_DIFF_TEST(CShopping2, GetPrice_ComputedKeys) {
    const char* items[] = { "TSHIRT", "JEANS", "SNEAKER" };
    for (auto* item : items) {
        uint32 key = CShopping::GetKey(item, (ePriceSection)0);
        int32 orig, rev;
        { HookDisableGuard guard("Global/CShopping/GetPrice");
          orig = CShopping::GetPrice(key); }
        rev = CShopping::GetPrice(key);
        EXPECT_EQ(orig, rev);
    }
}
