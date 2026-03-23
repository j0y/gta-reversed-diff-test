// test_CPedIntelligence2.cpp — Extended differential tests.
// Hook paths: Global/CPedIntelligence/<fn>

#include "StdInc.h"
#include "TestFramework.h"
#include "PedIntelligence.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- IsInSeeingRange ---

GAME_DIFF_TEST(CPedIntelligence2, IsInSeeingRange_NearPlayer) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* intel = player->GetIntelligence();
    if (!intel) return;
    CVector positions[] = {
        player->GetPosition() + CVector(5.0f, 0.0f, 0.0f),
        player->GetPosition() + CVector(50.0f, 0.0f, 0.0f),
        player->GetPosition() + CVector(200.0f, 0.0f, 0.0f),
        CVector(0.0f, 0.0f, 0.0f),
    };
    for (auto& pos : positions) {
        bool orig, rev;
        { HookDisableGuard guard("Global/CPedIntelligence/IsInSeeingRange"); orig = intel->IsInSeeingRange(pos); }
        rev = intel->IsInSeeingRange(pos);
        EXPECT_EQ(orig, rev);
    }
}

// --- FindRespectedFriendInInformRange ---

GAME_DIFF_TEST(CPedIntelligence2, FindRespectedFriendInInformRange) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* intel = player->GetIntelligence();
    if (!intel) return;
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/FindRespectedFriendInInformRange"); orig = intel->FindRespectedFriendInInformRange(); }
    rev = intel->FindRespectedFriendInInformRange();
    EXPECT_EQ(orig, rev);
}

// --- GetMoveStateFromGoToTask ---

GAME_DIFF_TEST(CPedIntelligence2, GetMoveStateFromGoToTask) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* intel = player->GetIntelligence();
    if (!intel) return;
    int32 orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/GetMoveStateFromGoToTask"); orig = (int32)intel->GetMoveStateFromGoToTask(); }
    rev = (int32)intel->GetMoveStateFromGoToTask();
    EXPECT_EQ(orig, rev);
}

// --- FindTaskByType (various task types) ---

GAME_DIFF_TEST(CPedIntelligence2, FindTaskByType_Various) {
    auto* player = FindPlayerPed(0);
    if (!player) return;
    auto* intel = player->GetIntelligence();
    if (!intel) return;
    eTaskType types[] = {
        TASK_NONE, TASK_SIMPLE_PLAYER_ON_FOOT, TASK_SIMPLE_STAND_STILL,
        TASK_COMPLEX_WANDER, TASK_COMPLEX_KILL_PED_ON_FOOT,
    };
    for (auto tt : types) {
        CTask* orig;
        CTask* rev;
        { HookDisableGuard guard("Global/CPedIntelligence/FindTaskByType"); orig = intel->FindTaskByType(tt); }
        rev = intel->FindTaskByType(tt);
        EXPECT_EQ((uintptr_t)orig, (uintptr_t)rev);
    }
}

// --- SpawnedPed tests ---

static Scenario::TestPed* s_intelPed = nullptr;
static bool s_triedIntelPed = false;

static CPed* GetIntelTestPed() {
    if (!s_triedIntelPed) {
        s_triedIntelPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_intelPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2410.0f, -1672.0f, 13.5f));
    }
    return s_intelPed ? s_intelPed->AsPed() : nullptr;
}

GAME_DIFF_TEST(CPedIntelligence2, IsInSeeingRange_SpawnedPed) {
    auto* ped = GetIntelTestPed();
    if (!ped) return;
    auto* intel = ped->GetIntelligence();
    if (!intel) return;
    CVector pos = ped->GetPosition() + CVector(10.0f, 0.0f, 0.0f);
    bool orig, rev;
    { HookDisableGuard guard("Global/CPedIntelligence/IsInSeeingRange"); orig = intel->IsInSeeingRange(pos); }
    rev = intel->IsInSeeingRange(pos);
    EXPECT_EQ(orig, rev);
}
