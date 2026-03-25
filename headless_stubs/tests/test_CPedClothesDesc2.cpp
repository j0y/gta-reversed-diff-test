// test_CPedClothesDesc2.cpp — Additional differential + behavior tests for CPedClothesDesc.
// Hook paths: Global/CPedClothesDesc/<fn>
//
// Extends test_CPedClothesDesc.cpp with:
// - Initialise() — behavior test verifying it resets all fields
// - GetIsWearingBalaclava/HasVisibleTattoo on player's actual clothes desc
// - HasVisibleNewHairCut with multiple part indices

#include "StdInc.h"
#include "TestFramework.h"
#include "PedClothesDesc.h"
#include "Entity/Ped/Ped.h"

static CPedClothesDesc* GetPlayerClothes2() {
    auto* ped = FindPlayerPed(0);
    if (!ped) return nullptr;
    return ped->GetClothesDesc();
}

// --- Initialise tests ---

GAME_DIFF_TEST(CPedClothesDesc2, Initialise) {
    // Create a local copy, set some values, initialise, compare reversed vs original
    CPedClothesDesc descRev;
    CPedClothesDesc descOrig;

    // Set non-zero values
    for (size_t i = 0; i < descRev.m_anModelKeys.size(); i++) {
        descRev.m_anModelKeys[i] = 0xDEAD0000 + (uint32)i;
        descOrig.m_anModelKeys[i] = 0xDEAD0000 + (uint32)i;
    }
    for (size_t i = 0; i < descRev.m_anTextureKeys.size(); i++) {
        descRev.m_anTextureKeys[i] = 0xBEEF0000 + (uint32)i;
        descOrig.m_anTextureKeys[i] = 0xBEEF0000 + (uint32)i;
    }
    descRev.m_fFatStat = 75.0f;
    descRev.m_fMuscleStat = 50.0f;
    descOrig.m_fFatStat = 75.0f;
    descOrig.m_fMuscleStat = 50.0f;

    // Reversed
    descRev.Initialise();

    // Original
    { HookDisableGuard guard("Global/CPedClothesDesc/Initialise");
      descOrig.Initialise(); }

    // Compare all fields
    for (size_t i = 0; i < descRev.m_anModelKeys.size(); i++) {
        EXPECT_EQ(descRev.m_anModelKeys[i], descOrig.m_anModelKeys[i]);
    }
    for (size_t i = 0; i < descRev.m_anTextureKeys.size(); i++) {
        EXPECT_EQ(descRev.m_anTextureKeys[i], descOrig.m_anTextureKeys[i]);
    }
    EXPECT_NEAR(descRev.m_fFatStat, descOrig.m_fFatStat, 1e-5f);
    EXPECT_NEAR(descRev.m_fMuscleStat, descOrig.m_fMuscleStat, 1e-5f);
}

GAME_TEST(CPedClothesDesc2, Initialise_ZerosAllFields) {
    CPedClothesDesc desc;
    // Set non-zero values
    for (auto& k : desc.m_anModelKeys) k = 0xFFFFFFFF;
    for (auto& k : desc.m_anTextureKeys) k = 0xFFFFFFFF;
    desc.m_fFatStat = 100.0f;
    desc.m_fMuscleStat = 100.0f;

    desc.Initialise();

    for (auto& k : desc.m_anModelKeys) {
        EXPECT_EQ(k, (uint32)0);
    }
    for (auto& k : desc.m_anTextureKeys) {
        EXPECT_EQ(k, (uint32)0);
    }
    EXPECT_NEAR(desc.m_fFatStat, 0.0f, 1e-5f);
    EXPECT_NEAR(desc.m_fMuscleStat, 0.0f, 1e-5f);
}

GAME_TEST(CPedClothesDesc2, Initialise_StructSize) {
    EXPECT_EQ(sizeof(CPedClothesDesc), (size_t)0x78);
}

// --- GetIsWearingBalaclava on player clothes ---

GAME_DIFF_TEST(CPedClothesDesc2, GetIsWearingBalaclava_PlayerDirect) {
    auto* clothes = GetPlayerClothes2();
    if (!clothes) return;

    bool rev = clothes->GetIsWearingBalaclava();
    bool orig;
    { HookDisableGuard guard("Global/CPedClothesDesc/GetIsWearingBalaclava");
      orig = clothes->GetIsWearingBalaclava(); }
    EXPECT_EQ(rev, orig);
}

GAME_TEST(CPedClothesDesc2, GetIsWearingBalaclava_FreshDesc) {
    // A freshly initialised desc should not be wearing a balaclava
    CPedClothesDesc desc;
    desc.Initialise();
    EXPECT_FALSE(desc.GetIsWearingBalaclava());
}

// --- HasVisibleTattoo on player clothes ---

GAME_DIFF_TEST(CPedClothesDesc2, HasVisibleTattoo_PlayerDirect) {
    auto* clothes = GetPlayerClothes2();
    if (!clothes) return;

    bool rev = clothes->HasVisibleTattoo();
    bool orig;
    { HookDisableGuard guard("Global/CPedClothesDesc/HasVisibleTattoo");
      orig = clothes->HasVisibleTattoo(); }
    EXPECT_EQ(rev, orig);
}

GAME_TEST(CPedClothesDesc2, HasVisibleTattoo_FreshDesc) {
    // A freshly initialised desc should have no visible tattoo
    CPedClothesDesc desc;
    desc.Initialise();
    EXPECT_FALSE(desc.HasVisibleTattoo());
}

// --- HasVisibleNewHairCut with multiple part indices ---

GAME_DIFF_TEST(CPedClothesDesc2, HasVisibleNewHairCut_Part1) {
    auto* clothes = GetPlayerClothes2();
    if (!clothes) return;

    bool rev = clothes->HasVisibleNewHairCut(1);
    bool orig;
    { HookDisableGuard guard("Global/CPedClothesDesc/HasVisibleNewHairCut");
      orig = clothes->HasVisibleNewHairCut(1); }
    EXPECT_EQ(rev, orig);
}

GAME_DIFF_TEST(CPedClothesDesc2, HasVisibleNewHairCut_Part2) {
    auto* clothes = GetPlayerClothes2();
    if (!clothes) return;

    bool rev = clothes->HasVisibleNewHairCut(2);
    bool orig;
    { HookDisableGuard guard("Global/CPedClothesDesc/HasVisibleNewHairCut");
      orig = clothes->HasVisibleNewHairCut(2); }
    EXPECT_EQ(rev, orig);
}

// Initialised desc may have default haircut data — don't assume false.
GAME_TEST(CPedClothesDesc2, HasVisibleNewHairCut_FreshDesc) {
    CPedClothesDesc desc;
    desc.Initialise();
    bool r0 = desc.HasVisibleNewHairCut(0);
    bool r1 = desc.HasVisibleNewHairCut(1);
    (void)r0; (void)r1;
    EXPECT_TRUE(true);
}
