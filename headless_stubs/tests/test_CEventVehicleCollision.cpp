// test_CEventVehicleCollision.cpp — Tests for CEventVehicleCollision.
// Hook paths: Events/CEventVehicleCollision/<fn>
// Tracks vehicle-ped collision data: piece type, damage intensity,
// impact vectors, and evasion type.

#include "StdInc.h"
#include "TestFramework.h"
#include "Events/EventVehicleCollision.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Ped/Ped.h"
#include "ScenarioHelpers.h"

static Scenario::TestVehicle* s_colVeh = nullptr;
static bool s_triedColVeh = false;

static CVehicle* GetCollisionTestVehicle() {
    if (!s_triedColVeh) {
        s_triedColVeh = true;
        int32 model = Scenario::FindLoadedVehicleModel();
        if (model >= 0) {
            s_colVeh = new Scenario::TestVehicle(model, CVector(2484.0f, -1668.0f, 13.5f));
        }
    }
    return s_colVeh ? s_colVeh->AsVehicle() : nullptr;
}

// --- Construction & type ---

GAME_TEST(CEventVehicleCollision, GetEventType) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventVehicleCollision event(0, 10.0f, nullptr, normal, pos, 0, VEHICLE_EVADE_NONE);
    EXPECT_EQ((int32)event.GetEventType(), (int32)EVENT_VEHICLE_COLLISION);
}

GAME_TEST(CEventVehicleCollision, GetEventPriority) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventVehicleCollision event(0, 10.0f, nullptr, normal, pos, 0, VEHICLE_EVADE_NONE);
    EXPECT_EQ(event.GetEventPriority(), 58);
}

GAME_TEST(CEventVehicleCollision, GetLifeTime) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventVehicleCollision event(0, 10.0f, nullptr, normal, pos, 0, VEHICLE_EVADE_NONE);
    EXPECT_EQ(event.GetLifeTime(), 0);
}

GAME_TEST(CEventVehicleCollision, Constructor_Fields) {
    CVector normal(1.0f, 2.0f, 3.0f), pos(4.0f, 5.0f, 6.0f);
    CEventVehicleCollision event(3, 25.5f, nullptr, normal, pos, 2, VEHICLE_EVADE_BY_HITSIDE);
    EXPECT_EQ(event.m_pieceType, (int16)3);
    EXPECT_NEAR(event.m_fDamageIntensity, 25.5f, 1e-5f);
    EXPECT_EQ((uintptr_t)event.m_vehicle, (uintptr_t)nullptr);
    EXPECT_NEAR(event.m_impactNormal.x, 1.0f, 1e-5f);
    EXPECT_NEAR(event.m_impactNormal.y, 2.0f, 1e-5f);
    EXPECT_NEAR(event.m_impactNormal.z, 3.0f, 1e-5f);
    EXPECT_NEAR(event.m_impactPos.x, 4.0f, 1e-5f);
    EXPECT_NEAR(event.m_impactPos.y, 5.0f, 1e-5f);
    EXPECT_NEAR(event.m_impactPos.z, 6.0f, 1e-5f);
    EXPECT_EQ(event.m_moveState, (int8)2);
    EXPECT_EQ(event.m_evadeType, (int16)VEHICLE_EVADE_BY_HITSIDE);
}

GAME_TEST(CEventVehicleCollision, TakesPriorityOver_AlwaysTrue) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    CEventVehicleCollision event1(0, 10.0f, nullptr, normal, pos, 0, VEHICLE_EVADE_NONE);
    CEventVehicleCollision event2(0, 5.0f, nullptr, normal, pos, 0, VEHICLE_EVADE_NONE);
    EXPECT_TRUE(event1.TakesPriorityOver(event2));
}

// --- Clone ---

GAME_TEST(CEventVehicleCollision, Clone_PreservesFields) {
    CVector normal(1.0f, 0.0f, 0.0f), pos(10.0f, 20.0f, 5.0f);
    CEventVehicleCollision original(2, 50.0f, nullptr, normal, pos, 1, VEHICLE_EVADE_NONE);
    CEvent* cloned = original.Clone();
    EXPECT_TRUE(cloned != nullptr);
    if (!cloned) return;

    EXPECT_EQ((int32)cloned->GetEventType(), (int32)EVENT_VEHICLE_COLLISION);

    auto* c = static_cast<CEventVehicleCollision*>(cloned);
    EXPECT_EQ(c->m_pieceType, (int16)2);
    EXPECT_NEAR(c->m_fDamageIntensity, 50.0f, 1e-5f);
    EXPECT_NEAR(c->m_impactNormal.x, 1.0f, 1e-5f);
    EXPECT_NEAR(c->m_impactPos.x, 10.0f, 1e-5f);
    EXPECT_NEAR(c->m_impactPos.y, 20.0f, 1e-5f);

    delete cloned;
}

// --- AffectsPed differential (requires vehicle) ---

GAME_DIFF_TEST(CEventVehicleCollision, AffectsPed_Player) {
    auto* veh = GetCollisionTestVehicle();
    if (!veh) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No vehicle"); return; }
    auto* ped = FindPlayerPed(0);
    if (!ped) { GetTestContext().RecordFailure(__FILE__, __LINE__, "No player ped"); return; }

    CVector normal(0, 1, 0), pos(2484.0f, -1668.0f, 13.5f);
    CEventVehicleCollision event(0, 5.0f, veh, normal, pos, 0, VEHICLE_EVADE_NONE);

    bool orig, rev;
    { HookDisableGuard guard("Events/CEventVehicleCollision/AffectsPed");
      orig = event.AffectsPed(ped); }
    rev = event.AffectsPed(ped);
    EXPECT_EQ(orig, rev);
}

// --- Different damage intensities ---

GAME_TEST(CEventVehicleCollision, DamageIntensityValues) {
    CVector normal(0, 0, 1), pos(0, 0, 0);
    float intensities[] = { 0.0f, 1.0f, 10.0f, 100.0f, 999.0f };
    for (float dmg : intensities) {
        CEventVehicleCollision event(0, dmg, nullptr, normal, pos, 0, VEHICLE_EVADE_NONE);
        EXPECT_NEAR(event.m_fDamageIntensity, dmg, 1e-5f);
    }
}
