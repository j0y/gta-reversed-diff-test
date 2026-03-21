// test_CEntity.cpp — Differential tests for CEntity/CPhysical member functions.
// Hook paths: Entity/CEntity/<fn> and Entity/CPhysical/<fn>
// Tests on FindPlayerPed(0) — the player ped is a CEntity/CPhysical.

#include "StdInc.h"
#include "TestFramework.h"
#include "Entity/Ped/Ped.h"
#include "World.h"

static CEntity* GetPlayerEntity() {
    return static_cast<CEntity*>(FindPlayerPed(0));
}

static CPhysical* GetPlayerPhysical() {
    return static_cast<CPhysical*>(FindPlayerPed(0));
}

// --- CEntity queries ---

#define DIFF_ENTITY_BOOL(FnName) \
    GAME_DIFF_TEST(CEntity, FnName) { \
        auto* ent = GetPlayerEntity(); \
        if (!ent) return; \
        bool orig, rev; \
        { HookDisableGuard guard("Entity/CEntity/" #FnName); orig = ent->FnName(); } \
        rev = ent->FnName(); \
        EXPECT_EQ(orig, rev); \
    }

DIFF_ENTITY_BOOL(GetIsOnScreen)
DIFF_ENTITY_BOOL(GetIsBoundingBoxOnScreen)
DIFF_ENTITY_BOOL(IsVisible)
DIFF_ENTITY_BOOL(GetIsStatic)
DIFF_ENTITY_BOOL(IsEntityOccluded)

GAME_DIFF_TEST(CEntity, GetDistanceFromCentreOfMassToBaseOfModel) {
    auto* ent = GetPlayerEntity();
    if (!ent) return;
    float orig, rev;
    { HookDisableGuard guard("Entity/CEntity/GetDistanceFromCentreOfMassToBaseOfModel");
      orig = ent->GetDistanceFromCentreOfMassToBaseOfModel(); }
    rev = ent->GetDistanceFromCentreOfMassToBaseOfModel();
    EXPECT_NEAR(orig, rev, 1e-3f);
}

GAME_DIFF_TEST(CEntity, GetColModel) {
    auto* ent = GetPlayerEntity();
    if (!ent) return;
    CColModel* orig;
    CColModel* rev;
    { HookDisableGuard guard("Entity/CEntity/GetColModel"); orig = ent->GetColModel(); }
    rev = ent->GetColModel();
    EXPECT_EQ(orig, rev);
}

// --- CPhysical queries ---

GAME_DIFF_TEST(CPhysical, GetSpeed) {
    auto* phys = GetPlayerPhysical();
    if (!phys) return;
    CVector speedOrig, speedRev;
    { HookDisableGuard guard("Entity/CPhysical/GetSpeed");
      speedOrig = phys->GetSpeed(CVector(0.0f, 0.0f, 0.0f)); }
    speedRev = phys->GetSpeed(CVector(0.0f, 0.0f, 0.0f));
    EXPECT_VEC_NEAR(speedOrig, speedRev, 1e-5f);
}

GAME_DIFF_TEST(CPhysical, GetHasCollidedWithAnyObject) {
    auto* phys = GetPlayerPhysical();
    if (!phys) return;
    bool orig, rev;
    { HookDisableGuard guard("Entity/CPhysical/GetHasCollidedWithAnyObject"); orig = phys->GetHasCollidedWithAnyObject(); }
    rev = phys->GetHasCollidedWithAnyObject();
    EXPECT_EQ(orig, rev);
}

GAME_DIFF_TEST(CPhysical, CanPhysicalBeDamaged) {
    auto* phys = GetPlayerPhysical();
    if (!phys) return;
    eWeaponType weapons[] = { WEAPON_UNARMED, WEAPON_PISTOL, WEAPON_EXPLOSION };
    for (auto wt : weapons) {
        bool orig, rev;
        { HookDisableGuard guard("Entity/CPhysical/CanPhysicalBeDamaged"); orig = phys->CanPhysicalBeDamaged(wt, nullptr); }
        rev = phys->CanPhysicalBeDamaged(wt, nullptr);
        EXPECT_EQ(orig, rev);
    }
}
