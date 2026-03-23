// test_CEventDeath.cpp — Tests for CEventDeath.
// CEventDeath extends CEvent directly (not editable response).
// Simple event representing ped death — drowning or otherwise.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventDeath.h"

// --- Construction & type ---

GAME_TEST(CEventDeath, GetEventType) {
    CEventDeath event(false);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_DEATH);
}

GAME_TEST(CEventDeath, GetEventPriority) {
    CEventDeath event(false);
    EXPECT_EQ(event.GetEventPriority(), 73);
}

GAME_TEST(CEventDeath, GetLifeTime) {
    CEventDeath event(false);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventDeath, Constructor_NotDrowning) {
    CEventDeath event(false);
    EXPECT_FALSE(event.HasDrowned());
}

GAME_TEST(CEventDeath, Constructor_Drowning) {
    CEventDeath event(true);
    EXPECT_TRUE(event.HasDrowned());
}

GAME_TEST(CEventDeath, Constructor_WithDeathTime) {
    CEventDeath event(true, 12345);
    EXPECT_TRUE(event.HasDrowned());
    EXPECT_EQ(event.GetDeathTime(), (uint32)12345);
}

GAME_TEST(CEventDeath, AffectsPed_AlwaysTrue) {
    CEventDeath event(false);
    // AffectsPed always returns true for CEventDeath
    EXPECT_TRUE(event.AffectsPed(nullptr));
}

// --- Clone ---

GAME_TEST(CEventDeath, Clone_PreservesFields) {
    CEventDeath original(true, 99999);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_DEATH);

    auto* c = static_cast<CEventDeath*>(cloned);
    EXPECT_TRUE(c->HasDrowned());
    EXPECT_EQ(c->GetDeathTime(), (uint32)99999);

    delete cloned;
}

GAME_TEST(CEventDeath, Clone_IsSeparateObject) {
    CEventDeath original(false);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    EXPECT_TRUE((uintptr_t)cloned != (uintptr_t)&original);
    delete cloned;
}
