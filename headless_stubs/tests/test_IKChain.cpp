// test_IKChain.cpp — Differential tests for IKChain_c.
// Hook paths: Global/IKChain_c/<fn>
//
// IKChain_c manages individual IK chains (look-at, point-arm, etc.) on peds.
// Active chains live in g_ikChainMan.m_IKChains[32]. We iterate active chains
// and test query functions on each.
//
// If no chains are active at state 9, we create one via AddIKChain.

#include "StdInc.h"
#include "TestFramework.h"
#include "IKChainManager_c.h"
#include "IKChain_c.h"
#include "Entity/Ped/Ped.h"

// Find ANY active IK chain on ANY ped (not just player)
static IKChain_c* FindAnyActiveChain() {
    for (auto& chain : g_ikChainMan.m_IKChains) {
        if (chain.GetPed() != nullptr) return &chain;
    }
    return nullptr;
}

// Try to create a look-at chain on the player ped
static IKChain_c* CreateTestChain() {
    auto* player = FindPlayerPed(0);
    if (!player) return nullptr;

    CVector lookTarget = player->GetPosition() + player->GetForward() * 5.0f;
    RwV3d lookPos = { lookTarget.x, lookTarget.y, lookTarget.z + 0.5f };
    RwV3d zero = { 0.f, 0.f, 0.f };

    return g_ikChainMan.AddIKChain(
        "test_look",
        eIKChainSlot::LOOK_AT,
        player,
        BONE_HEAD, zero,
        BONE_SPINE1,
        nullptr, BONE_UNKNOWN, lookPos,
        1.0f, 5
    );
}

static IKChain_c* GetOrCreateActiveChain() {
    auto* chain = FindAnyActiveChain();
    if (chain) return chain;
    return CreateTestChain();
}

// ======================================================================
// GetPriority
// ======================================================================

GAME_DIFF_TEST(IKChain, GetPriority) {
    auto* chain = GetOrCreateActiveChain();
    if (!chain) return;

    int8 orig, rev;
    { HookDisableGuard guard("Global/IKChain_c/GetPriority");
      orig = chain->GetPriority(); }
    rev = chain->GetPriority();
    EXPECT_EQ(orig, rev);
}

// IsFacingTarget and IsAtTarget crash in original code when the chain
// was found in the pool but not fully initialized (m_PivotBoneMatrix or
// bone world matrices are null). Skipped — these need a chain that was
// set up by the game's own LookAt system, not found by scanning the pool.

// ======================================================================
// GetBoneNodeFromTag — returns bone node pointer for a given tag
// ======================================================================

GAME_DIFF_TEST(IKChain, GetBoneNodeFromTag) {
    auto* chain = GetOrCreateActiveChain();
    if (!chain) return;

    eBoneTag32 tags[] = {
        BONE_HEAD, BONE_NECK, BONE_SPINE1, BONE_SPINE, BONE_PELVIS,
    };
    for (auto tag : tags) {
        BoneNode_c* orig;
        BoneNode_c* rev;
        { HookDisableGuard guard("Global/IKChain_c/GetBoneNodeFromTag");
          orig = chain->GetBoneNodeFromTag(tag); }
        rev = chain->GetBoneNodeFromTag(tag);
        // Both should return the same pointer (or both null)
        EXPECT_EQ(orig, rev);
    }
}

// ======================================================================
// GetLimits — get rotation limits for a bone on an axis
// ======================================================================

GAME_DIFF_TEST(IKChain, GetLimits) {
    auto* chain = GetOrCreateActiveChain();
    if (!chain) return;

    eBoneTag32 bones[] = { BONE_HEAD, BONE_NECK, BONE_SPINE1 };
    eRotationAxis axes[] = { AXIS_X, AXIS_Y, AXIS_Z };

    for (auto bone : bones) {
        // Only test bones that are in this chain
        if (!chain->GetBoneNodeFromTag(bone)) continue;

        for (auto axis : axes) {
            float origMin = 0.f, origMax = 0.f;
            float revMin = 0.f, revMax = 0.f;
            { HookDisableGuard guard("Global/IKChain_c/GetLimits");
              chain->GetLimits(bone, axis, origMin, origMax); }
            chain->GetLimits(bone, axis, revMin, revMax);
            EXPECT_NEAR(origMin, revMin, 1e-5f);
            EXPECT_NEAR(origMax, revMax, 1e-5f);
        }
    }
}

// ======================================================================
// SetBlend — compare raw bytes at m_Blend offset (private field)
// ======================================================================

GAME_DIFF_TEST(IKChain, SetBlend_RawBytes) {
    auto* chain = GetOrCreateActiveChain();
    if (!chain) return;

    // m_Blend is at offset 0x18 per VALIDATE_SIZE(IKChain_c, 0x58)
    // after: ListItem(8) + m_Ped(4) + m_BonesCount(4) + m_Bones(4) + m_PivotBoneMatrix(4) = 0x18
    constexpr int BLEND_OFFSET = 0x18;

    float blends[] = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };
    for (float b : blends) {
        { HookDisableGuard guard("Global/IKChain_c/SetBlend");
          chain->SetBlend(b); }
        float origBlend = *(float*)((uint8*)chain + BLEND_OFFSET);

        chain->SetBlend(b);
        float revBlend = *(float*)((uint8*)chain + BLEND_OFFSET);

        EXPECT_NEAR(origBlend, revBlend, 1e-5f);
    }
}

// ======================================================================
// SetOffsetPos — compare raw bytes
// ======================================================================

GAME_DIFF_TEST(IKChain, SetOffsetPos_RawBytes) {
    auto* chain = GetOrCreateActiveChain();
    if (!chain) return;

    // m_OffsetPos is after m_TargetEntity + m_OffsetBone
    // Exact offset: read the struct layout
    // ListItem(8) + Ped(4) + BonesCount(4) + Bones(4) + PivotBoneMat(4) + Blend(4)
    // + EffectorBone(2) + padding(2) + EffectorPos(12) + PivotBone(2) + padding(2)
    // + TargetEntity(4+4) + OffsetBone(4) = offset 0x38 for m_OffsetPos
    constexpr int OFFSET_POS_OFFSET = 0x38;

    CVector positions[] = {
        {0.f, 0.f, 0.f}, {1.f, 2.f, 3.f}, {-10.f, 5.f, 0.5f},
    };
    for (auto& pos : positions) {
        { HookDisableGuard guard("Global/IKChain_c/SetOffsetPos");
          chain->SetOffsetPos(pos); }
        CVector origPos = *(CVector*)((uint8*)chain + OFFSET_POS_OFFSET);

        chain->SetOffsetPos(pos);
        CVector revPos = *(CVector*)((uint8*)chain + OFFSET_POS_OFFSET);

        EXPECT_NEAR(origPos.x, revPos.x, 1e-5f);
        EXPECT_NEAR(origPos.y, revPos.y, 1e-5f);
        EXPECT_NEAR(origPos.z, revPos.z, 1e-5f);
    }
}

// ======================================================================
// Behavior tests
// ======================================================================

GAME_TEST(IKChain, ActiveChainExists) {
    // Count how many chains have a valid ped
    int activeCount = 0;
    for (auto& chain : g_ikChainMan.m_IKChains) {
        if (chain.GetPed() != nullptr) activeCount++;
    }

    auto* chain = GetOrCreateActiveChain();
    if (!chain) {
        char buf[128];
        _snprintf(buf, sizeof(buf),
            "No active IK chains found (%d with valid ped). "
            "AddIKChain also failed. Tests will be skipped.", activeCount);
        GetTestContext().RecordFailure(__FILE__, __LINE__, buf);
        return;
    }

    EXPECT_TRUE(chain->GetPed() != nullptr);
    EXPECT_TRUE(chain->GetPriority() >= 0);
}
