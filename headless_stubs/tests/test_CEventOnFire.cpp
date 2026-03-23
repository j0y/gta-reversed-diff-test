// test_CEventOnFire.cpp — Tests for CEventOnFire.
// CEventOnFire extends CEvent. Triggered when ped is on fire.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventOnFire.h"

// --- Construction & type ---

GAME_TEST(CEventOnFire, GetEventType) {
    CEventOnFire event;
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_ON_FIRE);
}

GAME_TEST(CEventOnFire, GetEventPriority) {
    CEventOnFire event;
    EXPECT_EQ(event.GetEventPriority(), 66);
}

GAME_TEST(CEventOnFire, GetLifeTime) {
    CEventOnFire event;
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventOnFire, GetLocalSoundLevel) {
    CEventOnFire event;
    EXPECT_NEAR(event.GetLocalSoundLevel(), 60.0f, 1e-5f);
}

// --- Clone ---

GAME_TEST(CEventOnFire, Clone_PreservesType) {
    CEventOnFire original;
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_ON_FIRE);
    EXPECT_EQ(cloned->GetEventPriority(), 66);

    delete cloned;
}

GAME_TEST(CEventOnFire, Clone_IsSeparateObject) {
    CEventOnFire original;
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}
