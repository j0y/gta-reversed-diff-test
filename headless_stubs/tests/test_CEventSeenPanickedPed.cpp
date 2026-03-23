// test_CEventSeenPanickedPed.cpp

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventSeenPanickedPed.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestPed* s_panicPed = nullptr;
static bool s_triedPanicPed = false;

static CPed* GetPanicTestPed() {
    if (!s_triedPanicPed) {
        s_triedPanicPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0)
            s_panicPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2452.0f, -1672.0f, 13.5f));
    }
    return s_panicPed ? s_panicPed->AsPed() : nullptr;
}

#define PANIC_GUARD() if (!GetPanicTestPed()) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No ped"); return; }

GAME_TEST(CEventSeenPanickedPed, GetEventType) {
    PANIC_GUARD()
    CEventSeenPanickedPed event(GetPanicTestPed());
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_SEEN_PANICKED_PED);
}

GAME_TEST(CEventSeenPanickedPed, GetEventPriority) {
    PANIC_GUARD()
    CEventSeenPanickedPed event(GetPanicTestPed());
    EXPECT_EQ(event.GetEventPriority(), 13);
}

GAME_TEST(CEventSeenPanickedPed, GetLifeTime) {
    PANIC_GUARD()
    CEventSeenPanickedPed event(GetPanicTestPed());
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventSeenPanickedPed, GetSourceEntity) {
    PANIC_GUARD()
    auto* ped = GetPanicTestPed();
    CEventSeenPanickedPed event(ped);
    EXPECT_EQ((uintptr_t)event.GetSourceEntity(), (uintptr_t)ped);
}
