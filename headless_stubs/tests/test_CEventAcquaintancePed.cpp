// test_CEventAcquaintancePed.cpp — Tests for CEventAcquaintancePed hierarchy.
// Tests the base class and two subclasses: Hate and Like.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventAcquaintancePed.h"
#include "Events/EventAcquaintancePedHate.h"
#include "Events/EventAcquaintancePedLike.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_acqPed = nullptr;
static bool s_triedAcqPed = false;

static CPed* GetAcquaintancePed() {
    if (!s_triedAcqPed) {
        s_triedAcqPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_acqPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2468.0f, -1672.0f, 13.5f));
    }
    return s_acqPed ? s_acqPed->AsPed() : nullptr;
}

#define ACQ_GUARD() if (!GetAcquaintancePed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

// --- Hate ---

GAME_TEST(CEventAcquaintancePedHate, GetEventType) {
    ACQ_GUARD()
    CEventAcquaintancePedHate event(GetAcquaintancePed());
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_ACQUAINTANCE_PED_HATE);
}

GAME_TEST(CEventAcquaintancePedHate, GetEventPriority) {
    ACQ_GUARD()
    CEventAcquaintancePedHate event(GetAcquaintancePed());
    EXPECT_EQ(event.GetEventPriority(), 26);
}

GAME_TEST(CEventAcquaintancePedHate, CanBeInterrupted) {
    ACQ_GUARD()
    CEventAcquaintancePedHate event(GetAcquaintancePed());
    EXPECT_TRUE(event.CanBeInterruptedBySameEvent());
}

GAME_TEST(CEventAcquaintancePedHate, GetSourceEntity) {
    ACQ_GUARD()
    auto* ped = GetAcquaintancePed();
    CEventAcquaintancePedHate event(ped);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)ped);
}

GAME_TEST(CEventAcquaintancePedHate, CloneEditable_PreservesPed) {
    ACQ_GUARD()
    auto* ped = GetAcquaintancePed();
    CEventAcquaintancePedHate original(ped);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_ACQUAINTANCE_PED_HATE);
    auto* c = static_cast<CEventAcquaintancePedHate*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_AcquaintancePed, (uintptr_t)ped);
    delete cloned;
}

// --- Like ---

GAME_TEST(CEventAcquaintancePedLike, GetEventType) {
    ACQ_GUARD()
    CEventAcquaintancePedLike event(GetAcquaintancePed());
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_ACQUAINTANCE_PED_LIKE);
}

GAME_TEST(CEventAcquaintancePedLike, GetEventPriority) {
    ACQ_GUARD()
    CEventAcquaintancePedLike event(GetAcquaintancePed());
    EXPECT_EQ(event.GetEventPriority(), 22);
}

GAME_TEST(CEventAcquaintancePedLike, CloneEditable_PreservesPed) {
    ACQ_GUARD()
    auto* ped = GetAcquaintancePed();
    CEventAcquaintancePedLike original(ped);
    auto* cloned = original.CloneEditable();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;
    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_ACQUAINTANCE_PED_LIKE);
    delete cloned;
}
