// test_CEventAcquaintancePed2.cpp — Tests for CEventAcquaintancePedDislike and Respect.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventAcquaintancePedDislike.h"
#include "Events/EventAcquaintancePedRespect.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_acqPed2 = nullptr;
static bool s_triedAcqPed2 = false;

static CPed* GetAcqPed2() {
    if (!s_triedAcqPed2) {
        s_triedAcqPed2 = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_acqPed2 = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2456.0f, -1672.0f, 13.5f));
    }
    return s_acqPed2 ? s_acqPed2->AsPed() : nullptr;
}

#define ACQ2_GUARD() if (!GetAcqPed2()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

// --- Dislike ---

GAME_TEST(CEventAcquaintancePedDislike, GetEventType) {
    ACQ2_GUARD()
    CEventAcquaintancePedDislike event(GetAcqPed2());
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_ACQUAINTANCE_PED_DISLIKE);
}

GAME_TEST(CEventAcquaintancePedDislike, GetEventPriority) {
    ACQ2_GUARD()
    CEventAcquaintancePedDislike event(GetAcqPed2());
    EXPECT_EQ(event.GetEventPriority(), 24);
}

GAME_TEST(CEventAcquaintancePedDislike, CloneEditable) {
    ACQ2_GUARD()
    auto* ped = GetAcqPed2();
    CEventAcquaintancePedDislike original(ped);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_ACQUAINTANCE_PED_DISLIKE);
    delete cloned;
}

// --- Respect ---

GAME_TEST(CEventAcquaintancePedRespect, GetEventType) {
    ACQ2_GUARD()
    CEventAcquaintancePedRespect event(GetAcqPed2());
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_ACQUAINTANCE_PED_RESPECT);
}

GAME_TEST(CEventAcquaintancePedRespect, GetEventPriority) {
    ACQ2_GUARD()
    CEventAcquaintancePedRespect event(GetAcqPed2());
    EXPECT_EQ(event.GetEventPriority(), 23);
}

GAME_TEST(CEventAcquaintancePedRespect, CloneEditable) {
    ACQ2_GUARD()
    auto* ped = GetAcqPed2();
    CEventAcquaintancePedRespect original(ped);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_ACQUAINTANCE_PED_RESPECT);
    delete cloned;
}
