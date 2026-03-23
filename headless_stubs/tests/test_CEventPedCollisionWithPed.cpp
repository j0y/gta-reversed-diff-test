// test_CEventPedCollisionWithPed.cpp — Tests for CEventPedCollisionWithPed.
// Tracks ped-on-ped collisions: body part, damage, velocity, positions,
// and movement states of both peds.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventPedCollisionWithPed.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

// --- Construction & type ---

GAME_TEST(CEventPedCollisionWithPed, GetEventType) {
    CVector vel(0, 0, 0), pos(0, 0, 0);
    CEventPedCollisionWithPed event(0, 5.0f, nullptr, vel, pos, PEDMOVE_WALK, PEDMOVE_STILL);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_PED_COLLISION_WITH_PED);
}

GAME_TEST(CEventPedCollisionWithPed, GetEventPriority) {
    CVector vel(0, 0, 0), pos(0, 0, 0);
    CEventPedCollisionWithPed event(0, 5.0f, nullptr, vel, pos, PEDMOVE_WALK, PEDMOVE_STILL);
    EXPECT_EQ(event.GetEventPriority(), 60);
}

GAME_TEST(CEventPedCollisionWithPed, GetLifeTime) {
    CVector vel(0, 0, 0), pos(0, 0, 0);
    CEventPedCollisionWithPed event(0, 5.0f, nullptr, vel, pos, PEDMOVE_WALK, PEDMOVE_STILL);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventPedCollisionWithPed, Constructor_Fields) {
    CVector vel(1.0f, 2.0f, 3.0f), pos(4.0f, 5.0f, 6.0f);
    CEventPedCollisionWithPed event(5, 15.0f, nullptr, vel, pos, PEDMOVE_RUN, PEDMOVE_SPRINT);
    EXPECT_EQ(event.m_pieceType, (int16)5);
    EXPECT_NEAR(event.m_damageIntensity, 15.0f, 1e-5f);
    EXPECT_NEAR(event.m_collisionImpactVelocity.x, 1.0f, 1e-5f);
    EXPECT_NEAR(event.m_collisionImpactVelocity.y, 2.0f, 1e-5f);
    EXPECT_NEAR(event.m_collisionImpactVelocity.z, 3.0f, 1e-5f);
    EXPECT_NEAR(event.m_collisionPos.x, 4.0f, 1e-5f);
    EXPECT_NEAR(event.m_collisionPos.y, 5.0f, 1e-5f);
    EXPECT_NEAR(event.m_collisionPos.z, 6.0f, 1e-5f);
}

GAME_TEST(CEventPedCollisionWithPed, MoveStateAccessors) {
    CVector vel(0, 0, 0), pos(0, 0, 0);
    CEventPedCollisionWithPed event(0, 5.0f, nullptr, vel, pos, PEDMOVE_RUN, PEDMOVE_SPRINT);
    EXPECT_EQ((int32)event.GetMoveState(), (int32)PEDMOVE_RUN);
    EXPECT_EQ((int32)event.GetVictimMoveState(), (int32)PEDMOVE_SPRINT);
}

// --- Clone ---

GAME_TEST(CEventPedCollisionWithPed, Clone_PreservesFields) {
    CVector vel(1.0f, 0.0f, 0.0f), pos(10.0f, 20.0f, 5.0f);
    CEventPedCollisionWithPed original(3, 30.0f, nullptr, vel, pos, PEDMOVE_WALK, PEDMOVE_RUN);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_PED_COLLISION_WITH_PED);

    auto* c = static_cast<CEventPedCollisionWithPed*>(cloned);
    EXPECT_EQ(c->m_pieceType, (int16)3);
    EXPECT_NEAR(c->m_damageIntensity, 30.0f, 1e-5f);
    EXPECT_NEAR(c->m_collisionImpactVelocity.x, 1.0f, 1e-5f);
    EXPECT_NEAR(c->m_collisionPos.x, 10.0f, 1e-5f);
    EXPECT_EQ((int32)c->GetMoveState(), (int32)PEDMOVE_WALK);
    EXPECT_EQ((int32)c->GetVictimMoveState(), (int32)PEDMOVE_RUN);

    delete cloned;
}

// --- With actual ped reference ---

static Scenario::TestPed* s_colPed = nullptr;
static bool s_triedColPed = false;

static CPed* GetCollisionTestPed() {
    if (!s_triedColPed) {
        s_triedColPed = true;
        int32 model = Scenario::FindLoadedPedModel();
        if (model >= 0) {
            s_colPed = new Scenario::TestPed(PED_TYPE_CIVMALE, model, CVector(2488.0f, -1672.0f, 13.5f));
        }
    }
    return s_colPed ? s_colPed->AsPed() : nullptr;
}

GAME_TEST(CEventPedCollisionWithPed, Clone_WithVictim) {
    auto* victim = GetCollisionTestPed();
    if (!victim) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No spawned ped"); return; }

    CVector vel(0, 1, 0), pos(0, 0, 0);
    CEventPedCollisionWithPed original(0, 5.0f, victim, vel, pos, PEDMOVE_WALK, PEDMOVE_STILL);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    auto* c = static_cast<CEventPedCollisionWithPed*>(cloned);
    EXPECT_EQ((uintptr_t)c->m_victim, (uintptr_t)victim);

    delete cloned;
}

// --- Damage intensity coverage ---

GAME_TEST(CEventPedCollisionWithPed, DamageIntensityValues) {
    CVector vel(0, 0, 0), pos(0, 0, 0);
    float intensities[] = { 0.0f, 1.0f, 10.0f, 50.0f, 100.0f };
    for (float dmg : intensities) {
        CEventPedCollisionWithPed event(0, dmg, nullptr, vel, pos, PEDMOVE_WALK, PEDMOVE_STILL);
        EXPECT_NEAR(event.m_damageIntensity, dmg, 1e-5f);
    }
}
