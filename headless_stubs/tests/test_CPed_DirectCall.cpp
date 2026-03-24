// test_CPed_DirectCall.cpp — Direct-call differential tests for CPed::GetBonePosition.
// void __thiscall GetBonePosition(CVector* out, eBoneTag bone, bool updateSkin)
// Address: 0x5E4280

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

GAME_TEST(CPed_DirectCall, GetBonePosition_Player_CommonBones) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    int32 bones[] = { 1, 2, 3, 4, 5, 6, 7, 8, 21, 22, 23, 24, 31, 32, 33, 34 };
    for (int32 bone : bones) {
        CVector orig{}, rev{};
        CallOriginal_OutVec_Thiscall(0x5E4280, player, &orig, bone, false);
        player->GetBonePosition(&rev, static_cast<eBoneTag>(bone), false);
        EXPECT_NEAR(orig.x, rev.x, 1e-2f);
        EXPECT_NEAR(orig.y, rev.y, 1e-2f);
        EXPECT_NEAR(orig.z, rev.z, 1e-2f);
    }
}

GAME_TEST(CPed_DirectCall, GetBonePosition_Player_WithSkinUpdate) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    int32 bones[] = { 1, 4, 8, 22, 32 };
    for (int32 bone : bones) {
        CVector orig{}, rev{};
        CallOriginal_OutVec_Thiscall(0x5E4280, player, &orig, bone, true);
        player->GetBonePosition(&rev, static_cast<eBoneTag>(bone), true);
        EXPECT_NEAR(orig.x, rev.x, 1e-2f);
        EXPECT_NEAR(orig.y, rev.y, 1e-2f);
        EXPECT_NEAR(orig.z, rev.z, 1e-2f);
    }
}

// --- Spawned NPC ---

static Scenario::TestPed* s_dcPed = nullptr;
static bool s_triedDcPed = false;

static CPed* GetDcPed() {
    if (!s_triedDcPed) {
        s_triedDcPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_dcPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2496.f, -1670.f, 13.5f));
    }
    return s_dcPed ? s_dcPed->AsPed() : nullptr;
}

GAME_TEST(CPed_DirectCall, GetBonePosition_NPC_CommonBones) {
    auto* ped = GetDcPed();
    if (!ped) return;
    int32 bones[] = { 1, 2, 3, 4, 5, 6, 7, 8, 21, 22, 23, 24, 31, 32, 33, 34 };
    for (int32 bone : bones) {
        CVector orig{}, rev{};
        CallOriginal_OutVec_Thiscall(0x5E4280, ped, &orig, bone, false);
        ped->GetBonePosition(&rev, static_cast<eBoneTag>(bone), false);
        EXPECT_NEAR(orig.x, rev.x, 1e-2f);
        EXPECT_NEAR(orig.y, rev.y, 1e-2f);
        EXPECT_NEAR(orig.z, rev.z, 1e-2f);
    }
}
