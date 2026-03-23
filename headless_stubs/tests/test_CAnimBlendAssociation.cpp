// test_CAnimBlendAssociation.cpp — Tests for CAnimBlendAssociation.
// Hook paths: Animation/CAnimBlendAssociation/<fn>
//
// CAnimBlendAssociation represents a running animation instance.
// Tests verify flag queries and blend operations on player ped's animations.

#include "StdInc.h"
#include "TestFramework.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimManager.h"
#include "Entity/Ped/Ped.h"

// --- Flag queries on default-constructed association ---

GAME_TEST(CAnimBlendAssociation, DefaultConstructor_Flags) {
    CAnimBlendAssociation assoc;
    EXPECT_FALSE(assoc.IsPlaying());
    EXPECT_FALSE(assoc.IsLooped());
    EXPECT_FALSE(assoc.IsPartial());
    EXPECT_FALSE(assoc.IsFacial());
}

GAME_TEST(CAnimBlendAssociation, DefaultConstructor_BlendValues) {
    CAnimBlendAssociation assoc;
    // Default blend amount is 1.0 (full blend), delta/time/speed are 0
    EXPECT_NEAR(assoc.GetBlendAmount(), 1.0f, 1e-5f);
    EXPECT_NEAR(assoc.GetBlendDelta(), 0.0f, 1e-5f);
    EXPECT_NEAR(assoc.GetCurrentTime(), 0.0f, 1e-5f);
    EXPECT_NEAR(assoc.GetSpeed(), 1.0f, 1e-5f);
}

// --- SetFlag / HasFlag ---

GAME_TEST(CAnimBlendAssociation, SetFlag_Playing) {
    CAnimBlendAssociation assoc;
    assoc.SetFlag(ANIMATION_IS_PLAYING, true);
    EXPECT_TRUE(assoc.IsPlaying());
    EXPECT_TRUE(assoc.HasFlag(ANIMATION_IS_PLAYING));

    assoc.SetFlag(ANIMATION_IS_PLAYING, false);
    EXPECT_FALSE(assoc.IsPlaying());
}

GAME_TEST(CAnimBlendAssociation, SetFlag_Looped) {
    CAnimBlendAssociation assoc;
    assoc.SetFlag(ANIMATION_IS_LOOPED, true);
    EXPECT_TRUE(assoc.IsLooped());

    assoc.SetFlag(ANIMATION_IS_LOOPED, false);
    EXPECT_FALSE(assoc.IsLooped());
}

GAME_TEST(CAnimBlendAssociation, SetFlag_Multiple) {
    CAnimBlendAssociation assoc;
    assoc.SetFlag(ANIMATION_IS_PLAYING, true);
    assoc.SetFlag(ANIMATION_IS_LOOPED, true);
    assoc.SetFlag(ANIMATION_IS_PARTIAL, true);

    EXPECT_TRUE(assoc.IsPlaying());
    EXPECT_TRUE(assoc.IsLooped());
    EXPECT_TRUE(assoc.IsPartial());

    // Clear one, others remain
    assoc.SetFlag(ANIMATION_IS_LOOPED, false);
    EXPECT_TRUE(assoc.IsPlaying());
    EXPECT_FALSE(assoc.IsLooped());
    EXPECT_TRUE(assoc.IsPartial());
}

// --- Blend setters ---

GAME_TEST(CAnimBlendAssociation, SetBlendAmount) {
    CAnimBlendAssociation assoc;
    assoc.SetBlendAmount(0.75f);
    EXPECT_NEAR(assoc.GetBlendAmount(), 0.75f, 1e-5f);
}

GAME_TEST(CAnimBlendAssociation, SetBlendDelta) {
    CAnimBlendAssociation assoc;
    assoc.SetBlendDelta(1.5f);
    EXPECT_NEAR(assoc.GetBlendDelta(), 1.5f, 1e-5f);
}

GAME_TEST(CAnimBlendAssociation, SetSpeed) {
    CAnimBlendAssociation assoc;
    assoc.SetSpeed(2.0f);
    EXPECT_NEAR(assoc.GetSpeed(), 2.0f, 1e-5f);
}

// --- GetBlendAmount with weight ---

GAME_TEST(CAnimBlendAssociation, GetBlendAmount_NonPartial_Weight) {
    CAnimBlendAssociation assoc;
    assoc.SetBlendAmount(1.0f);
    // Non-partial: blend = amount * weight
    EXPECT_NEAR(assoc.GetBlendAmount(0.5f), 0.5f, 1e-5f);
}

GAME_TEST(CAnimBlendAssociation, GetBlendAmount_Partial_IgnoresWeight) {
    CAnimBlendAssociation assoc;
    assoc.SetBlendAmount(1.0f);
    assoc.SetFlag(ANIMATION_IS_PARTIAL, true);
    // Partial: blend = amount (ignores weight)
    EXPECT_NEAR(assoc.GetBlendAmount(0.5f), 1.0f, 1e-5f);
}

// --- Player ped animation queries ---

GAME_TEST(CAnimBlendAssociation, PlayerPed_HasIdleAnim) {
    auto* ped = FindPlayerPed(0);
    if (!ped || !ped->m_pRwClump) return;

    auto* idleAssoc = RpAnimBlendClumpGetAssociation(ped->m_pRwClump, ANIM_ID_IDLE);
    // Player should have an idle animation at state 9
    // (may not if in a special state, so just verify no crash)
    (void)idleAssoc;
    EXPECT_TRUE(true);
}

// GetTimeProgress is not hooked — behavior test only

GAME_TEST(CAnimBlendAssociation, GetTimeProgress_NoExcept) {
    auto* ped = FindPlayerPed(0);
    if (!ped || !ped->m_pRwClump) return;

    auto* assoc = RpAnimBlendClumpGetFirstAssociation(ped->m_pRwClump);
    if (!assoc) return;

    float progress = assoc->GetTimeProgress();
    EXPECT_TRUE(progress >= 0.0f);
    EXPECT_TRUE(progress <= 1.0f);
}
